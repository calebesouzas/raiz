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
#include "dynamic_arrays.h"
#include "types.h"
#include "value.h"
#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "scope.h"
#include "runtime.h"
#include "program.h"

void print_errs(SemanticError_A *errs, char *file_path, size_t source_len);
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
  Program pro = Program_setup(scope, &par);

  err = Program_build(&pro);
  if (err)
    return err;

  SemanticError_A errs = {0};
  Program_check(&pro, &errs, 20);
  if (errs.len > 0) {
    print_errs(&errs, argv[1], code.len);
    return (int)errs.len;
  }
  free(errs.dat);

  (void) Program_run(&pro);

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

void print_errs(SemanticError_A *errs, char *file_path, size_t source_len) {
#define SPEC "%s: [%zu](%zu) error:\n"
#define DAT file_path, e->token->start, e->token->line
#define TOK size_t_int(e->token->len), e->token->lexeme
#define P(...) fprintf(stderr, __VA_ARGS__)
#define TYPE0\
  size_t_int(e->type[0]->pattern.name.len), e->type[0]->pattern.name.ptr
#define TYPE1\
  size_t_int(e->type[1]->pattern.name.len), e->type[1]->pattern.name.ptr
#define TYPE_PAT\
  (e->type_pattern.ptr_count + e->type_pattern.name.len),\
  _type_pattern_to_str(e->type_pattern)

  SemanticError *e;

  da_for(e, errs) {
    if (e->token == NULL)
      e->token = e->expr->token;

    switch (e->code) {
    case ERR_SEM_UNDEFINED_SYMBOL:
      P(SPEC"undefined symbol '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ALREADY_DECLARED_SYMBOL:
      P(SPEC"already declared symbol '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ASSIGN_TO_VAL:
      P(SPEC"assigned to value '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ASSIGN_TO_RVALUE:
      P(SPEC"assigned to R-value '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_DECL_AFTER_IF_ELSE:
      P(SPEC"declaration after if-else\n", DAT);
      break;
    case ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE:
      P(SPEC"declaration after while-then-else\n", DAT);
      break;
    case ERR_SEM_INCOMPATIBLE_TYPES:
      P(SPEC"incompatible types: '@%.*s' and '@%.*s'\n", DAT, TYPE0, TYPE1);
      break;
    case ERR_SEM_LOOP_KEYWORD_OUTSIDE_LOOP:
      P(SPEC"used '%.*s' keyword outside loop\n", DAT, TOK);
      break;
    case ERR_SEM_INCOMPATIBLE_OPERATOR:
      P(SPEC"type '%.*s' doesn't work with '%.*s' operator\n", DAT, TYPE0, TOK);
      break;
    case ERR_SEM_UNDEFINED_TYPE:
      P(SPEC"undefined type '@%.*s'\n", DAT, TYPE_PAT);
      break;
    }

    char *s = e->token->lexeme;
    // find line beginning
    if (e->token->line > 1) {
      while (*s != '\n') s--;
      s++;
    } else {
      s -= e->token->start;
    }

    // get line length
    int i = -1;
    while (s[++i] != '\n' && i < (source_len - e->token->start));

    fprintf(stderr,
      "%zu | %.*s\n\n",
      e->token->line, i, s);
  }
}

#include "lexer.c"
#include "parser.c"
#include "scope.c"
#include "strings.c"
#include "program.c"
#include "runtime.c"
#include "types.c"
