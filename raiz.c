#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
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
  size_t len;
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
  size_t current, start, len;
} Lexer;

//////// LEXER (functions) ////////
static inline Lexer lexer_init(char*const source) {
  return (Lexer) { .source = source, .current = 0, .start = 0 };
}

static inline void lexer_backup(Lexer* lexer) {
  lexer->start = lexer->current;
}

static inline void lexer_restore(Lexer* lexer) {
  lexer->current = lexer->start;
}

struct LexerAdvance_opt {
  size_t amount;
  char expected;
};

// HACK: optional parameters!
#define lexer_advance(lexer, ...)\
  lexer_advance_opt((lexer), (struct LexerAdvance_opt) { __VA_ARGS__ })

// TODO: add checking for equality between a 'char expected' param and the
// current character, if not equal, panic on that (in debug version only)!
static inline void lexer_advance_opt(Lexer* lexer, struct LexerAdvance_opt opt) {
  if (opt.expected) assert(lexer->source[lexer->current] == opt.expected);

  if (!opt.amount) {
    lexer->current++;
    lexer->len++;
  }
  else {
    lexer->current += opt.amount;
    lexer->len = lexer->current - lexer->start;
  }
}

#define lexer_make_token(lexer, token_kind, ...)\
  lexer_make_token_opt((lexer), (Token) {\
    .kind = (token_kind),\
    .len = (lexer)->current - (lexer)->start,\
    __VA_ARGS__\
  })

static inline Token lexer_make_token_opt(Lexer* l, Token opt) {
  // TODO: implement position spans
  return opt;
}

static inline bool is_white_space(char c) {
  return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

#define current() (lexer->source[lexer->current])
Token lexer_next_token(Lexer* lexer) {
  if (lexer->source == NULL || current() == '\0')
    return lexer_make_token(lexer, TOKEN_END_OF_FILE);

  switch (current()) {
  case ' ':
  case '\n':
  case '\t':
  case '\r':
    while (is_white_space(current())) lexer_advance(lexer);
    return lexer_next_token(lexer);
  case '+':
    lexer_advance(lexer);
    return lexer_make_token(lexer, TOKEN_OPERATOR, .data.op = OP_SUM);
  case '-':
    lexer_advance(lexer);
    return lexer_make_token(lexer, TOKEN_OPERATOR, .data.op = OP_SUBTRACT);
  case '*':
    lexer_advance(lexer);
    return lexer_make_token(lexer, TOKEN_OPERATOR, .data.op = OP_MULTIPLY);
  case '/':
    lexer_advance(lexer);
    return lexer_make_token(lexer, TOKEN_OPERATOR, .data.op = OP_DIVIDE);
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
    lexer_backup(lexer);
    int number = 0;
    for (; isdigit(current()); lexer_advance(lexer))
      number = (number * 10) + (current() - '0');

    return lexer_make_token(lexer, TOKEN_LITERAL_NUMBER, .data.value = number);
  default:
    if (g_raiz_error_buffer[0] != '\0') // means it is not empty
      memset(g_raiz_error_buffer, 0, RAIZ_ERROR_BUFFER_CAPACITY);

    // Here we are sure that the buffer will not overflow
    sprintf(g_raiz_error_buffer, "unrecognized character: '%c'", current());
    return lexer_make_token(lexer, TOKEN_ERROR);
  }

  return lexer_make_token(lexer, TOKEN_END_OF_FILE);
}
#undef current

Token lexer_peek_token(Lexer* lexer) {
  lexer_backup(lexer);
  Token token = lexer_next_token(lexer);
  lexer_restore(lexer);
  return token;
}

//////// PARSER (types) ////////
typedef enum {
  EXPR_ERROR,
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
  Token current;
  Token peeked;
} Parser;

//////// PARSER (functions) ////////
uint8_t parser_get_binding_power(Operator op) {
  // HACK: we use bitshifts to put two numbers in one single byte since we
  // don't need large numbers.
  switch (op) {
  case OP_SUM:
  case OP_SUBTRACT:
    return 2 + (1 << 4);
  case OP_MULTIPLY:
  case OP_DIVIDE:
    return 4 + (3 << 4);
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

ExprArena expr_arena_init(size_t init_capacity) {
  Expr* pool = malloc(sizeof(Expr) * init_capacity);
  if (!pool) PANIC("failed to allocate expressions memory pool\n");

  return (ExprArena) {.count = 0, .capacity = init_capacity, .items = pool};
}

void parser_push_expr(Parser* parser, Expr* expr) {
  if (parser->ast.count >= parser->ast.capacity) {
    LOG("reallocating expression arena\n");
    parser->ast.capacity *= 1.5;
    parser->ast.items = realloc(parser->ast.items, parser->ast.capacity);
  }
  expr->id = parser->ast.count;
  parser->ast.current = expr->id;
  parser->ast.items[parser->ast.count++] = *expr;
  LOG("pushing expression (id: %u)", expr->id);
}

/* Function parser_parse_literal:
 * Pushes an 'Expr' of kind EXPR_LITERAL into 'parser->ast' and 
 * returns the pointer to the pushed expression (in 'parser->ast').
 * If not, returns 'NULL' */
Expr* parser_push_literal(Parser* parser) {
  Expr number = expr_number(parser->current.data.value);
  parser_push_expr(parser, &number);

  return &parser->ast.items[parser->ast.count - 1];
}

/* Function parser_parse_binary:
 * Pushes 'Expr' of kind 'EXPR_BINARY' with 'op', 'left' and 'right'.
 * After that, returns pointer to the pushed expression.
 * Otherwise, returns 'NULL'.
 * If 'push_all' is 'true', pushes 'left' and 'right' into 'parser->ast' before
 * */
Expr* parser_push_binary(
  Parser* parser, Expr* left, Expr* right, Operator op, bool push_all
) {
  if (push_all) {
    parser_push_expr(parser, left);
    parser_push_expr(parser, right);
  }
  Expr result = expr_binary(left, right, op);
  parser_push_expr(parser, &result);
  return &parser->ast.items[parser->ast.count - 1];
}

#define current() (parser->current)
#define peek() (parser->peeked = lexer_peek_token(parser->lexer))
#define next() (parser->current = lexer_next_token(parser->lexer))
#define advance() lexer_advance(parser->lexer, .amount = parser->peeked.len)
#define last_expr() (parser->ast.items[parser->ast.count-1])
#define get_expr(index) (parser->ast.items[(index)])
#define split_powers(left_name, right_name, bind_powers)\
  uint8_t left_name;\
  uint8_t right_name;\
  do {\
    uint8_t pair = (bind_powers);\
    left_name = (pair) >> 4;\
    right_name = ((pair) - left_name) >> 4;\
  } while (0)

bool parser_parse_expr(Parser* parser, uint8_t min_bp) {
  while (1) {
    LOG("in loop\n");
    switch (next().kind) {
    case TOKEN_ERROR:
      PANIC("%s\n", g_raiz_error_buffer);
    case TOKEN_END_OF_FILE: return false;
    case TOKEN_LITERAL_NUMBER:
      LOG("returning literal\n");
      parser_push_literal(parser);
      return true;
    case TOKEN_OPERATOR:
      // NOTE: we need to find a way to get the last expression's right side
      // and bind it to this expression's left side if this expression's
      // operator has higher binding power than last's...
      if (peek().kind != TOKEN_LITERAL_NUMBER) PANIC("expected number\n");
      LOG("found operator\n");
      Operator op = current().data.op;
      advance(); // consume peeked number literal

      split_powers(left_bp, right_bp, parser_get_binding_power(op));
      if (left_bp < min_bp) {
        LOG("breaking\n");
        return true;
      }
      advance(); // consume operator

      LOG("mounting binary (id: %d)\n", last_expr().id);
      size_t right_id = parser_parse_expr(parser, right_bp);
      Expr left = expr_binary(last_expr(), get_expr(right_id), op);
      parser_push_expr(parser, &left);

      break;
    default: UNREACHABLE("token kind");
    }
  }
end:
  LOG("at end\n");
  return true;
}


ExprArena parser_parse(Lexer* lexer) {
  Parser parser = { .lexer = lexer, .ast = expr_arena_init(256) };

  while (parser_parse_expr(&parser, 0) != parser.ast.count);

  return parser.ast;
}
#undef current
#undef peek
#undef next
#undef advance
#undef last_expr
#undef get_expr

//////// COMPILER (functions) ////////
ExprArena build_ast(char*const code) {
  Lexer lexer = lexer_init(code);
  return parser_parse(&lexer);
}

//////// EVALUATOR (functions) ////////
#define current() arena->items[arena->current]
void log_eval(ExprArena* arena, size_t indent) {
  indent++;
  
  for (size_t i = indent; i > 0; i--) printf(" ");

  switch (current().kind) {
  case EXPR_LITERAL:
    // TODO: put expression number value into 'buffer'
    printf("Literal: %d\n", current().as.literal);
    break;
  case EXPR_BINARY:
    printf("Binary");
    switch(current().as.binary.op) {
    case OP_SUM:
      printf("(+):\n");
      break;
    case OP_SUBTRACT:
      printf("(-):\n");
      break;
    case OP_MULTIPLY:
      printf("(*):\n");
      break;
    case OP_DIVIDE:
      printf("(/):\n");
      break;
    }
    arena->current = current().as.binary.left_id;
    log_eval(arena, indent);

    arena->current = current().as.binary.right_id;
    log_eval(arena, indent);
    break;
  }
}

int eval(ExprArena* arena) {
  LOG("eval(): current %d\n", arena->current);
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

int main(int argc, char* argv[]) {
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

  if (argc > 1 && strcmp(argv[1], "inter") == 0) {
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
  } else {
    char code[] = "2 + 3 * 4";
    ExprArena ast = build_ast(code);
    // return 0;
    printf("Result: %d\n", eval(&ast));
    ast.current = ast.count - 1;
    log_eval(&ast, 0);

    free(ast.items);
  }

  return 0;
}
