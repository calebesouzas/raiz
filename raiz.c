#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
/*
 Warning: this codebase is probably terrible...
But maybe you can find something interesting in here, I really don't know
*/

#include "dynamic_arrays.h"
#include "lexer.h"
#include "parser.h"

int eval(Expr *e);

int main(int argc, char **argv) {
#if 0
  if (argc <= 1) {
    printf("usage: %s <file>\n", argv[0]);
    printf("<file>: text file containing Raiz code\n");
    return 1;
  }
#endif
  Token_A toks = {0};
  int err = Lexer_tokenize(&toks, "20 - 5 + 10 / 2 * 3\n");
  if (err)
    return err;

  AST ast = {0};
  err = Parser_build_ast(&ast, &toks);
  if (err)
    return err;

  fprintf(stderr, "----------\n");
  int result = eval(ast.root);
  printf("%d\n", result);
  return 0;
}

int eval(Expr *e) {
  if (!e) {
    fprintf(stderr, "expression is null\n");
    return 0;
  }
  fprintf(stderr, "kind = %d\n", e->kind);

  switch (e->kind) {
  case EXPR_LITERAL:
    return e->literal;
  case EXPR_UNARY:
    switch (e->unary.op.kind) {
    case TOKEN_MINUS:
      return -eval(e->unary.in);
    }
  case EXPR_BINARY: {
    fprintf(stderr, "left side\n");
    int ls = eval(e->binary.ls);
    fprintf(stderr, "right side\n");
    int rs = eval(e->binary.rs);
    fprintf(stderr, "operator %s\n", token_label(&e->binary.op));
    switch (e->binary.op.kind) {
    case TOKEN_PLUS:
      return ls + rs;
    case TOKEN_MINUS:
      return ls - rs;
    case TOKEN_STAR:
      return ls * rs;
    case TOKEN_SLASH:
      return ls / rs;
    }
  } break;
  }
  return 0;
}

#include "lexer.c"
#include "parser.c"
