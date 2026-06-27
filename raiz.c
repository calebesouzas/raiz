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

int main(int argc, char **argv) {
#if 0
  if (argc <= 1) {
    printf("usage: %s <file>\n", argv[0]);
    printf("<file>: text file containing Raiz code\n");
    return 1;
  }
#endif
  Token_A toks = {0};
  int err = Lexer_tokenize(&toks, code);
  if (err)
    return err;

  Scope *scope = Scope_();
  Parser par = Parser_setup(&toks);
  Program pro = Program_setup(scope, &par);

  err = Program_build(&pro);
  if (err)
    return err;

  int result = Program_run(&pro);
  printf("%d\n", result);
  return 0;
}

#include "lexer.c"
#include "parser.c"
#include "scope.c"
#include "program.c"
#include "runtime.c"
