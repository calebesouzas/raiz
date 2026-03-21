#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////// DEBUGING ////////
#define DEBUG

#ifdef DEBUG
#define log_file stderr

#define LOG(...)\
  fprintf(log_file, __VA_ARGS__)

#else
#define LOG(...)
#endif

//////// ERRORS ////////
#define RAIZ_ERROR_BUFFER_CAPACITY (1024)
// NOTE: i'm not sure if using a global buffer to store error messages is a good
// idea. But i think it is (because we don't need to pass a pointer 
// to in in every single function that can fail for some reason)
char g_raiz_error_buffer[RAIZ_ERROR_BUFFER_CAPACITY] = {0};

#define PANIC(...) do { fprintf(stderr, __VA_ARGS__); exit(1); } while (0)

#define TODO(message)\
  do { fprintf(stderr, "TODO: %s\n", message); abort(); } while (0)

#define UNREACHABLE(message)\
  PANIC("reached unreachable code at %s:%d: \"%s\"",\
        __FILE__, __LINE__, message)

//////// UTILITIES ////////
#ifndef DA_INIT_CAPACITY
#define DA_INIT_CAPACITY (256)
#endif

#define da_append(array, value)\
  do {\
    if (!(array)->items) {\
      (array)->items = malloc(sizeof(*(array)->items) * DA_INIT_CAPACITY);\
      if ((array)->items != NULL) {\
        (array)->capacity = DA_INIT_CAPACITY;\
        (array)->count = 0;\
      }\
    }\
    if ((array)->count >= (array)->capacity) {\
      (array)->capacity *= 1.5;\
      (array)->items = realloc(\
        (array)->items, sizeof(*(array)->items) * (array)->capacity\
      );\
    }\
    (array)->items[(array)->count++] = (value);\
  } while (0)

#define da_free(array) (array)->items ? free((array)->items) : ;

//////// LEXER (types) ////////
typedef enum {
  OP_SUM,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
} Operator;

typedef enum {
  // in case of unrecognized byte or invalid character in current context
  TOKEN_ERROR,
  TOKEN_OPERATOR,
  TOKEN_LITERAL_NUMBER,
  TOKEN_END_OF_FILE
} TokenKind;

typedef struct {
  TokenKind kind;
  union {
    Operator op;
    int value;
  } data;
} Token;

typedef struct {
  size_t count;
  size_t capacity;
  Token *items;
} Tokens;

typedef struct {
  const char* source;
  char* current;
  char* start;
  size_t len;
} Lexer;

//////// LEXER (functions) ////////
static inline Lexer lexer_init(char*const source) {
  return (Lexer) { .source = source, .current = source, .start = source };
}

static inline void lexer_backup(Lexer* l) {
  l->start = l->current;
}

struct LexerAdvance_opt {
  size_t amount;
  char expected;
};

// HACK: optional parameters!
#define lexer_advance(l, ...)\
  lexer_advance_opt(l, (struct LexerAdvance_opt) { __VA_ARGS__ })

// TODO: add checking for equality between a 'char expected' param and the
// current character, if not equal, panic on that (in debug version only)!
static inline void lexer_advance_opt(Lexer* l, struct LexerAdvance_opt opt) {
  if (opt.expected) assert(*l->current == opt.expected);

  if (!opt.amount) {
    l->current++;
    l->len++;
  }
  else {
    l->current += opt.amount;
    l->len += opt.amount;
  }
}

#define lexer_make_token(lexer, token_kind, ...)\
  lexer_make_token_opt((lexer), (Token) {.kind = (token_kind), __VA_ARGS__})

static inline Token lexer_make_token_opt(Lexer* l, Token opt) {
  // TODO: implement position spans
  return opt;
}

static inline bool is_white_space(char c) {
  return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

Token lexer_next_token(Lexer* l) {
  if (l->current == NULL || *l->current == '\0')
    return lexer_make_token(l, TOKEN_END_OF_FILE);

  switch (*l->current) {
  case ' ':
  case '\n':
  case '\t':
  case '\r':
    while (is_white_space(*l->current)) lexer_advance(l);
    return lexer_next_token(l);
  case '+':
    lexer_advance(l);
    return lexer_make_token(l, TOKEN_OPERATOR, .data.op = OP_SUM);
  case '-':
    lexer_advance(l);
    return lexer_make_token(l, TOKEN_OPERATOR, .data.op = OP_SUBTRACT);
  case '*':
    lexer_advance(l);
    return lexer_make_token(l, TOKEN_OPERATOR, .data.op = OP_MULTIPLY);
  case '/':
    lexer_advance(l);
    return lexer_make_token(l, TOKEN_OPERATOR, .data.op = OP_DIVIDE);
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    lexer_backup(l);
    int number = 0;
    for (; isdigit(*l->current); lexer_advance(l))
      number = (number * 10) + (*l->current - '0');

    return lexer_make_token(l, TOKEN_LITERAL_NUMBER, .data.value = number);
  default:
    if (g_raiz_error_buffer[0] != '\0') // means it is not empty
      memset(g_raiz_error_buffer, 0, RAIZ_ERROR_BUFFER_CAPACITY);

    // Here we are sure that the buffer will not overflow
    sprintf(g_raiz_error_buffer, "unrecognized character: '%c'", *l->current);
    return lexer_make_token(l, TOKEN_ERROR);
  }

  return lexer_make_token(l, TOKEN_END_OF_FILE);
}

Tokens lexer_tokenize(Lexer* l) {
  Tokens tokens = {0};
  for (;;) {
    Token token = lexer_next_token(l);
    da_append(&tokens, token);
    if (token.kind == TOKEN_END_OF_FILE) break;
    else if (token.kind == TOKEN_ERROR) PANIC("%s\n", g_raiz_error_buffer);
  }
  return tokens;
}

//////// PARSER (types) ////////
typedef enum {
  EXPR_BINARY,
  EXPR_LITERAL,
} ExprKind;

typedef struct {
  ExprKind kind;
  size_t id; // index relative to 'ExprArena.items' array at 'Parser.ast'
  union {
    struct {
      size_t left_id;
      size_t right_id;
      Operator op;
    } binary;
    int literal;
  } as;
} Expr;

// NOTE: the '.items[0]' is reserved for the top expression.
// This is the last one we mount. But the first one to be 'eval()'ed
typedef struct {
  size_t count, capacity;
  size_t current; // use at 'eval()' only!
  Expr* items;
} ExprArena;

typedef struct {
  Lexer *lexer;
  ExprArena ast;
} Parser;

//////// PARSER (functions) ////////

uint8_t get_binding_power(Operator op) {
  switch (op) {
  case OP_SUM:
  case OP_SUBTRACT:
    return 1;
  case OP_MULTIPLY:
  case OP_DIVIDE:
    return 2;
  default: UNREACHABLE("operator");
  }
}

// 'Expr' generators (helpers)
#define expr_number(number)\
  (Expr){.kind = EXPR_LITERAL, .as.literal = (number)}

#define expr_binary(left, right, op)\
  (Expr) {\
    .kind = EXPR_BINARY,\
    .as.binary = {\
      .left_id = (left).id,\
      .right_id = (right).id,\
      .op = (op),\
    },\
  }

ExprArena arena_init(size_t init_capacity) {
  Expr* pool = malloc(sizeof(Expr) * init_capacity);
  if (!pool) PANIC("failed to allocate expressions memory pool\n");

  return (ExprArena) {.count = 0, .capacity = init_capacity, .items = pool};
}

void parser_push_expr(Parser* parser, Expr* expr) {
  if (parser->ast.count >= parser->ast.capacity) {
    parser->ast.capacity *= 1.5;
    parser->ast.items = realloc(parser->ast.items, parser->ast.capacity);
  }
  expr->id = parser->ast.count;
  parser->ast.current = expr->id;
  parser->ast.items[parser->ast.count++] = *expr;
}

ExprArena parser_parse(Lexer* lexer) {
#define current() tokens.items[index]
#define peek() tokens.items[index - 1]
#define next() tokens.items[index--]
#define advance() index--
#define last_expr() parser.ast.items[parser.ast.count-1]

  Parser parser = { .lexer = lexer, .ast = arena_init(256)};
  Tokens tokens = lexer_tokenize(parser.lexer);

  for (int index = tokens.count - 1; index > 0; index--) {
    Token token = current();
    // printf("at index %u (kind id: %d)\n", index, token.kind);
    switch (token.kind) {
    case TOKEN_ERROR:
      PANIC("%s\n", g_raiz_error_buffer);
      break;
    case TOKEN_OPERATOR:
      // If it's literal, it is simple, we just need to bind it to any side(?)
      // of a binary expression.
      // But, if it's a binary expression, we need to know which operator has
      // the greater binding power between the current expression we're
      // mounting and the last one we already added to the arena
      if (last_expr().kind == EXPR_LITERAL) {
        Token next_token = peek();
        if (next_token.kind != TOKEN_LITERAL_NUMBER) PANIC("expected number\n");
        advance();
 
        Expr right = expr_number(token.data.value);
        Expr left = last_expr();
        Operator op = token.data.op;
        Expr parent = expr_binary(left, right, op);
        parser_push_expr(&parser, &parent);
      } else if (last_expr().kind == EXPR_BINARY) {
        uint8_t current_bp = get_binding_power(token.data.op);
        uint8_t child_bp = get_binding_power(last_expr().as.binary.op);
        if (current_bp > child_bp) {
          TODO("capture inner expression");
        } else {
          TODO("mount binary with binary children");
        }
      }
      break;
    case TOKEN_LITERAL_NUMBER:
      Expr number = expr_number(token.data.value);
      parser_push_expr(&parser, &number);
      break;
    case TOKEN_END_OF_FILE:
      break;
    }
  }
  free(tokens.items);
  return parser.ast;
#undef current
#undef peek
#undef next
#undef advance
#undef last_expr
}


//////// COMPILER (functions) ////////
ExprArena build_ast(char*const code) {
  Lexer lexer = lexer_init(code);
  return parser_parse(&lexer);
}

//////// EVALUATOR (functions) ////////
void log_eval(ExprArena* arena, size_t indent) {
  indent++;
  
  for (size_t i = indent; i > 0; i--) printf(" ");

  switch (arena->items[arena->current].kind) {
  case EXPR_LITERAL:
    // TODO: put expression number value into 'buffer'
    printf("Literal: %d\n", arena->items[arena->current].as.literal);
    break;
  case EXPR_BINARY:
    printf("Binary:\n");
    size_t parent_id = arena->current;
    arena->current = arena->items[parent_id].as.binary.left_id;
    log_eval(arena, indent);

    arena->current = arena->items[parent_id].as.binary.right_id;
    log_eval(arena, indent);
    break;
  }
}

int eval(ExprArena* arena) {
#define current() arena->items[arena->current]

  switch (current().kind) {
  case EXPR_LITERAL: return current().as.literal;
  case EXPR_BINARY:
    // save id for later use
    size_t parent_id = arena->current;

    arena->current = current().as.binary.left_id;
    int left = eval(arena);

    arena->current = current().as.binary.right_id;
    int right = eval(arena);

    switch (arena->items[parent_id].as.binary.op) {
    case OP_SUM: return left + right;
    case OP_SUBTRACT: return left - right;
    case OP_MULTIPLY: return left * right;
    case OP_DIVIDE: return left / right;
    }
  }
  UNREACHABLE("eval");
  // return 0;
#undef current
}

int main(void) {
  // char code[] = "56 + 8 / 2 - 7 * 3";
  // Following precedence:
  // 56 + 8 / 2 - 7 * 3
  // 56 + 4 - 21
  // 60 - 21
  // 39
  // From left to right only (what's actually happening):
  // 56 + 8 / 2 - 7 * 3
  // 64 / 2 - 7 * 3
  // 32 - 7 * 3
  // 25 * 3
  // 75

  char buffer[1024] = {0};
  printf("> ");
  while (fgets(buffer, sizeof(buffer), stdin)) {
    ExprArena ast = build_ast(buffer);
    printf("Result: %d\n", eval(&ast));
    ast.current = ast.count - 1;
    log_eval(&ast, 0);

    free(ast.items);
    printf("> ");
  }

  return 0;
}
