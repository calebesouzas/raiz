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

#define TODO(message) do { fprintf(stderr, "TODO: %s\n", message); abort(); } while (0)

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
  return (c == ' ' || c == '\t' || c == '\r');
}

Token lexer_next_token(Lexer* l) {
  if (l->current == NULL || *l->current == '\0')
    return lexer_make_token(l, TOKEN_END_OF_FILE);

  switch (*l->current) {
  case ' ':
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
    sprintf(g_raiz_error_buffer, "unrecognized character: %c", *l->current);
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
  size_t left_index;
  size_t right_index;
  Operator op;
  int value; // if kind == LITERAL
} Expr;

// NOTE: the '.items[0]' is reserved for the top expression.
// This is the last one we mount. But the first one to be 'eval()'ed
typedef struct {
  size_t count, capacity;
  Expr* items;
} ExprArena;

typedef struct {
  Lexer *lexer;
  ExprArena ast;
} Parser;

//////// PARSER (functions) ////////
#define expr_number(number) (Expr) { .kind = EXPR_LITERAL, .value = (number) }

void parser_push_expr(Parser* parser, Expr expr) {
  da_append(parser->ast, expr);
}

Expr* parser_parse(Lexer* lexer) {
#define current() tokens.items[index]
#define peek() tokens.items[index - 1]
#define next() tokens.items[index--]
#define advance() index--

  Parser parser = { .lexer = lexer, .ast = {0} };
  Tokens tokens = lexer_tokenize(parser.lexer);

  for (size_t index = tokens.count - 1; index >= 0; index--) {
    Token token = current();
    switch (token.kind) {
    case TOKEN_ERROR:
      PANIC(g_raiz_error_buffer);
      break;
    case TOKEN_OPERATOR:
      // Shouldn't arrive here...
      TODO("implement unary expressions");
      break;
    case TOKEN_LITERAL_NUMBER:
      Expr right = expr_number(token.data.value);
      Token next_token;
      if ((next_token = peek()).kind == OPERATOR)) {
        advance();
        Operator op = next_token.data.op;
        next_token = peek();
        if (next_token.kind != TOKEN_LITERAL) PANIC("expected number");
        Expr left = next_token.data.value;
        advance();

      }

      break;
    case TOKEN_END_OF_FILE:
      break;
    }
  }
#undef parser_current
#undef parser_peek
#undef parser_next
#undef parser_advance
}


//////// COMPILER (functions) ////////
// Expr* build_ast(char* const code) {
//   Lexer lexer = lexer_init(code);
//
//   return parser_parse(&lexer);
// }

//////// EVALUATOR (functions) ////////
int eval(Expr* node) {
  if (node->kind == EXPR_BINARY) {
    int lhs;
    int rhs;
    if (node->lhs) {
      lhs = eval(node->lhs);
    }
    if (node->rhs) {
      rhs = eval(node->rhs);
    } else {
      return lhs;
    }
    switch (node->op) {
    case OP_SUM: return lhs + rhs;
    case OP_SUBTRACT: return lhs - rhs;
    case OP_MULTIPLY: return lhs * rhs;
    case OP_DIVIDE: return lhs / rhs;
    }
  }

  // if a literal expression:
  return node->value;
}

int main(void) {
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
  char code[] = "56 + 8 / 2 - 7 * 3";

  Lexer lexer = lexer_init(code);
  Tokens tokens = lexer_tokenize(&lexer);
  // Expr* ast = build_ast(code);

  // assert(ast->kind == EXPR_BINARY);
  // assert(ast->lhs->kind == EXPR_LITERAL);
  // assert(ast->rhs->kind == EXPR_LITERAL);

  // printf("Result: %d\n", eval(ast));

  // parser_free_node(ast);

  return 0;
}
