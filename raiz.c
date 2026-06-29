#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/*
 Warning: this codebase is probably terrible...
But maybe you can find something interesting in here, I really don't know
*/

char code[] = {
  #embed "test_code.rz"
  , 0
};

#include "macros.h"
#include "dynamic_arrays.h"
#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "scope.h"
#include "runtime.h"
#include "program.h"

void print_errs(SemanticError_A *errs);
int main(int argc, char **argv) {
#if 0
  if (argc <= 1) {
    printf("usage: %s <file>\n", argv[0]);
    printf("<file>: text file containing Raiz code\n");
    return 1;
  }
#endif
  Token_A toks = {0};
  Lexer lex = Lexer_setup(&toks, code);
  int err = Lexer_tokenize(&lex);
  if (err)
    return err;

  Scope *scope = Scope_();
  Parser par = Parser_setup(&toks);
  Program pro = Program_setup(scope, &par);

  err = Program_build(&pro);
  if (err)
    return err;

  SemanticError_A errs = {0};
  Program_check(&pro, &errs, 20);
  if (errs.len > 0) {
    print_errs(&errs);
    return (int)errs.len;
  }
  free(errs.dat);

  int result = Program_run(&pro);
  printf("%d\n", result);

  Program_free(&pro);
  return 0;
}

void print_errs(SemanticError_A *errs) {
#define SPEC "error at [%zu] (%zu:%zu): "
#define DAT e->token->start, e->token->line, e->token->column
#define TOK e->token->len, e->token->source
#define P(...) fprintf(stderr, __VA_ARGS__)

  SemanticError *e;
  da_for(e, errs) {
    switch (e->code) {
    case ERR_SEM_UNDEFINED_SYMBOL:
      P(SPEC"undefined symbol '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ALREADY_DECLARED_SYMBOL:
      P(SPEC"symbol '%.*s' already declared in this scope\n", DAT, TOK);
      break;
    case ERR_SEM_ASSIGN_TO_VAL:
      P(SPEC"attempt to reassign to value '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ASSIGN_TO_RVALUE:
      P(SPEC"attempt to assign to rvalue\n", DAT);
      break;
    }
  }
}

#include "lexer.c"
#include "parser.c"
#include "scope.c"
#include "program.c"
#include "runtime.c"
