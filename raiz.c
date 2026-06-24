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

#include "dynamic_arrays.h"
#include "lexer.h"
#include "parser.h"
#include "runtime.h"

int main(int argc, char **argv) {
#if 0
  if (argc <= 1) {
    printf("usage: %s <file>\n", argv[0]);
    printf("<file>: text file containing Raiz code\n");
    return 1;
  }
#endif
  Token_A toks = {0};
  // (5 + 4) * 2
  // 9 * 2
  // 18
  int err = Lexer_tokenize(&toks, "a\n");
  if (err)
    return err;

  Expr ast = {0};
  err = Parser_build_ast(&ast, &toks);
  if (err)
    return err;

  Symbol a = {0};
  strncpy(a.ident, "a", sizeof(a.ident));
  a.value = 69;
  Symbol_A symbols = {0};
  da_add(&symbols, a);
  int result = eval(&ast, &symbols);
  printf("%d\n", result);
  return 0;
}

#include "lexer.c"
#include "parser.c"
#include "runtime.c"
