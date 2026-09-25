#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

#include "expr.h"

typedef struct {
  ExprNode_A ast;
  Token_A *toks;
  size_t cur;
  ExprNode_A errs;
} Parser;

enum ParserError {
  PARSER_INVALID_TOKEN,
  PARSER_UNEXPECTED_TOKEN,
  PARSER_EXPECTATION_FAILED,
};

Expr *Expr_(void);

Expr *Expr_copy(Expr *src);

void Expr_free(Expr *node);

void Expr_dump(Expr *root, size_t indent, size_t level);

Token *Parser_current(Parser *par);
Token *Parser_peek(Parser *par);
Token *Parser_next(Parser *par);
Token *Parser_advance(Parser *par);

Parser Parser_setup(Token_A *toks);

int Parser_parse_nud(Expr *res, Parser *par);
int Parser_parse_expr(Expr *res, Parser *par, uint8_t min_bp);
int Parser_parse_line(Expr *res, Parser *par);
int Parser_parse_type(TypePattern *res, Parser *par);

uint8_t get_binding_power(enum TokenKind kind);

int Parser_build_ast(Expr *ast, Token_A *toks);

#endif // RAIZ_PARSER_H
