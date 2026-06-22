#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

struct Expr {
  enum {
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
  } kind;
  union {
    int literal;
    struct {
      Token op;
      struct Expr *ls;
      struct Expr *rs;
    } binary;
    struct {
      Token op;
      struct Expr *in;
    } unary;
  };
};

typedef struct Expr Expr;

typedef struct {
  Expr *root;
} AST;

typedef struct {
  AST *ast;
  Token_A *toks;
  size_t cur;
} Parser;

enum ParserError {
  PARSER_EXPECTED_OPERATOR = 1,
  PARSER_INVALID_TOKEN,
  PARSER_UNEXPECTED_TOKEN,
};

Expr *Expr_(void);

Expr *Expr_copy(Expr *src);

Token Parser_cur(Parser *par);
Token Parser_peek(Parser *par);
Token Parser_next(Parser *par);
Token Parser_advance(Parser *par);

int Parser_parse_nud(Expr *res, Parser *par);
int Parser_parse_expr(Expr *res, Parser *par, uint8_t min_bp);

void binding_power_of(Token *op, uint8_t *lbp, uint8_t *rbp);

int Parser_build_ast(AST *ast, Token_A *toks);

#endif // RAIZ_PARSER_H
