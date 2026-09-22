#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

#include "expr.h"

typedef struct {
  Expr *ast;
  Token_A *toks;
  size_t cur;
} Parser;

enum ParserError {
  PARSER_EXPECTED_OPERATOR = 1,
  PARSER_INVALID_TOKEN,
  PARSER_UNEXPECTED_TOKEN,
  PARSER_TRAILING_PARENTHESES,
  PARSER_EXPECTED_IDENTIFIER,
  PARSER_EXPECTED_ASSIGNMENT,
  PARSER_EXPECTED_FINISH,
  PARSER_EXPECTED_EXPRESSION,
  PARSER_EXPECTED_TYPE,
  PARSER_NOT_CLOSED_GROUP,
  PARSER_NOT_CLOSED_BLOCK,
  PARSER_EXPECTED_DECLARATOR,
  PARSER_EXPECTED_COLLON,
};

Expr *Expr_(void);

Expr *Expr_copy(Expr *src);

void Expr_free(Expr *node);

void Expr_dump(Expr *root, size_t indent, size_t level);

Token *Parser_cur(Parser *par);
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
