#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

#include "common.h"

typedef struct
{
  Rz_Lexer *lexer;
  Rz_Token current;
  Rz_Token next;
  Rz_ExprArena *arena;
  // Do we really need it? I think it's better than 'PANIC()'ing everywhere...
  char *error_buffer;
} Rz_Parser;

Rz_Expr *rz_parser_build_ast(Rz_ExprArena *arena, const char *source);

#endif // RAIZ_PARSER_H
