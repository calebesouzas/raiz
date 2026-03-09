#ifndef RAIZ_PARSER_STATE_H
#define RAIZ_PARSER_STATE_H

#include "../lexer/tokens.h"
#include "statements.h"

typedef struct {
  unsigned int index;
  Token* tokens;
  Stmt* stmts;
} ParserState;

#endif // RAIZ_PARSER_STATE_H
