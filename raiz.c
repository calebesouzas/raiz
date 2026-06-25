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

#include "macros.h"
#include "dynamic_arrays.h"
#include "lexer.h"
#include "parser.h"
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
  int err = Lexer_tokenize(&toks, "var x\nval y = 32\nx = y * 2");
  if (err)
    return err;

  Symbol_A symbols = {0};
  Parser par = Parser_setup(&toks);
  Program pro = Program_setup(&symbols, &par);

  err = Program_build(&pro, &par);
  if (err)
    return err;

  int result = Program_run(&pro);
  fprintf(stderr, "; symbol count = %zu\n", symbols.len);
  printf("%d\n", result);
  return 0;
}

#include "lexer.c"
#include "parser.c"
#include "program.c"
#include "runtime.c"
