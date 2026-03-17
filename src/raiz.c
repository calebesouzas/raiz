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

//////// LEXER (types) ////////
typedef enum {
  // in case of unrecognized byte or invalid character in current context
  TOKEN_ERROR,
  TOKEN_PLUS,
  TOKEN_LITERAL_NUMBER,
  TOKEN_END_OF_FILE
} TokenKind;

typedef struct {
  TokenKind kind;
  int value;
} Token;

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

static inline Token lexer_make_token(Lexer* l, TokenKind kind) {
  // TODO: implement position spans
  return (Token) {.kind = kind};
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
    return lexer_make_token(l, TOKEN_PLUS);
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
    // TODO: fix repeated digits being skipped
    lexer_backup(l);
    int number = *l->current - '0';
    while (isdigit(*(++l->current))) {
      number = (number * 10) + (*l->current - '0');
      lexer_advance(l);
    }
    LOG("Found number: %d\n", number);

    return (Token) {.kind = TOKEN_LITERAL_NUMBER, .value = number};
  default:
    if (g_raiz_error_buffer[0] != '\0') // means it is not empty
      memset(g_raiz_error_buffer, 0, RAIZ_ERROR_BUFFER_CAPACITY);

    // Here we are sure that the buffer will not overflow
    sprintf(g_raiz_error_buffer, "unrecognized character: %c", *l->current);
    return lexer_make_token(l, TOKEN_ERROR);
  }

  return lexer_make_token(l, TOKEN_END_OF_FILE);
}

//////// PARSER (types) ////////
typedef enum {
  EXPR_BINARY,
  EXPR_LITERAL,
} ExprKind;

typedef struct Expr Expr;
struct Expr {
  ExprKind kind;
  struct Expr* lhs;
  struct Expr* rhs;
  int value; // if kind == LITERAL
};

typedef struct {
  Lexer *lex;
  Expr *ast; // one single for now
} Parser;

//////// PARSER (functions) ////////
static inline Parser parser_init(Lexer* lex) {
  Expr* ast = malloc(sizeof(*ast));
  assert(ast != NULL);
  return (Parser) {.lex = lex, .ast = ast};
}

// TODO: finish handler for simple '+' operations and number literals
// TODO: free() nodes after use!
bool parser_next_node(Parser* p) {
  Token tok = lexer_next_token(p->lex);
  switch (tok.kind) {
  case TOKEN_ERROR:
    PANIC("%s\n", g_raiz_error_buffer);
  case TOKEN_PLUS:
    lexer_advance(p->lex); // consume '+'
    Token next_tok = lexer_next_token(p->lex);
    if (next_tok.kind != TOKEN_LITERAL_NUMBER) PANIC("expected number");

    Expr* rhs = malloc(sizeof(*rhs));
    if (rhs) {
      rhs->kind = EXPR_LITERAL;
      rhs->value = next_tok.value;
    }

    // Save current state of AST into left side so we don't lose it
    Expr* lhs = malloc(sizeof(*lhs));
    assert(lhs != NULL);
    memcpy(lhs, p->ast, sizeof(*p->ast));

    p->ast->kind = EXPR_BINARY;
    p->ast->rhs = rhs;
    return true;
  case TOKEN_LITERAL_NUMBER:
    if (!p->ast->lhs) p->ast->lhs = malloc(sizeof(*p->ast));
    if (p->ast->lhs)
      (*p->ast->lhs) = (Expr) {.kind = EXPR_LITERAL, .value = tok.value};

    return true;
  case TOKEN_END_OF_FILE: return false;
  }
}

Expr* parser_parse(Lexer* l) {
  Parser p = parser_init(l);

  while (parser_next_node(p));

  return p->ast;
}

void parser_free_node(Expr* e) {
  if (e) {
    if (e->lhs) parser_free_node(e->lhs);
    if (e->rhs) parser_free_node(e->rhs);
    free(e);
    e = NULL;
  }
}

//////// COMPILER (functions) ////////
Expr* build_ast(char* const code) {
  Lexer lexer = lexer_init(code);

  return parser_parse(&lexer);
}

//////// EVALUATOR (functions) ////////
int eval(Expr* node) {
  if (node->kind == EXPR_BINARY) {
    int lhs = eval(node->lhs);
    int rhs = eval(node->rhs);
    return lhs + rhs;
  }

  // if a literal expression:
  return node->value;
}

int main(void) {
  char code[] = "69 + 44 + 1337";
  Expr* ast = build_ast(code);

  assert(ast->kind == EXPR_BINARY);
  assert(ast->lhs->kind == EXPR_LITERAL);
  assert(ast->rhs->kind == EXPR_LITERAL);

  printf("Result: %d\n", eval(ast));

  parser_free_node(ast);

  return 0;
}
