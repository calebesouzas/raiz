#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/*
 Warning: this codebase is probably terrible...
But maybe you can find something interesting in here, I really don't know
*/

#include "strings.h"

#include "cast.h"
#include "macros.h"
#include "debug.h"
#include "dynamic_arrays.h"
#include "types.h"
#include "value.h"
#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "scope.h"
#include "runtime.h"
#include "semantics.h"
#include "program.h"

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("usage: %s <file>\n", argv[0]);
    printf("<file>: text file containing Raiz code\n");
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  fseek(file, 0, SEEK_END);

  struct {
    size_t len, cap;
    char *dat;
  } code;
  code.cap = ftell(file);
  code.dat = malloc(code.cap);

  fseek(file, 0, SEEK_SET);
  code.len = fread(code.dat, sizeof(*code.dat), code.cap, file);

  if (feof(file))
    printf("reached end, read %zu bytes\n", code.len);
  else if (ferror(file))
    printf("error ocurred");

  fclose(file);
  file = NULL;

  Token_A toks = {0};
  Lexer lex = Lexer_setup(&toks, code.dat, code.len);
  int err = Lexer_tokenize(&lex);
  if (err)
    return err;

  Scope *scope = Scope_();
  Parser par = Parser_setup(&toks);

  Scope_insert_builtins(scope);
  Program pro = Program_setup(scope, &par);

  err = Program_build(&pro);
  if (err)
    return err;

  SemanticError_A errs = {0};
  Program_check(&pro, &errs, 20);
  if (errs.len > 0) {
    Semantics_print_errs(&errs, argv[1], code.len);
    return (int)errs.len;
  }
  free(errs.dat);

  Value res = Program_run(&pro);
  Value_print(&res, true);

  Program_free(&pro);
  free(code.dat);
  return 0;
}

static char *_type_pattern_to_str(TypePattern pattern) {
  size_t i;
  for (i = 0; i < pattern.ptr_count; i++) {
    Temp_Buffer[i] = '*';
  }

  strncpy(Temp_Buffer + i, pattern.name.ptr, pattern.name.len);

  return Temp_Buffer;
}

#include "lexer.c"
#include "parser.c"
#include "value.c"
#include "scope.c"
#include "semantics.c"
#include "strings.c"
#include "program.c"
#include "runtime.c"
#include "types.c"
