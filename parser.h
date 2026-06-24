#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

struct Expr {
  enum {
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_GROUP,
    EXPR_IDENT,
    EXPR_DECL,
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
    struct {
      struct Expr *in;
    } group;
    char ident[TOKEN_IDENTIFIER_SIZE];
    struct {
      uint32_t kind;
      struct Expr *value;
      char ident[TOKEN_IDENTIFIER_SIZE];
    } decl;
  };
};

typedef struct Expr Expr;

typedef struct {
  Expr *ast;
  Token_A *toks;
  size_t cur;
} Parser;

enum ParserError {
  PARSER_EXPECTED_OPERATOR = 1,
  PARSER_INVALID_TOKEN,
  PARSER_UNEXPECTED_TOKEN,
  PARSER_NON_CLOSED_GROUP,
  PARSER_TRAILING_PARENTHESES,
  PARSER_EXPECTED_IDENTIFIER,
  PARSER_EXPECTED_ASSIGNMENT,
  PARSER_EXPECTED_FINISH,
};

Expr *Expr_(void);

Expr *Expr_copy(Expr *src);

void Expr_dump(Expr *root, size_t indent, size_t level);

Token Parser_cur(Parser *par);
Token Parser_peek(Parser *par);
Token Parser_next(Parser *par);
Token Parser_advance(Parser *par);

Parser Parser_setup(Token_A *toks);
void Parser_debug(Parser *par);

int Parser_parse_nud(Expr *res, Parser *par);
int Parser_parse_expr(Expr *res, Parser *par, uint8_t min_bp);

void binding_power_of(Token *op, uint8_t *lbp, uint8_t *rbp);

int Parser_build_ast(Expr *ast, Token_A *toks);

#endif // RAIZ_PARSER_H
