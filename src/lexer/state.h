#ifndef RAIZ_LEXER_STATE_H
#define RAIZ_LEXER_STATE_H

#include "tokens.h"
#include "raiz_arrays.h"

typedef struct {
  unsigned int index, start, line, column;
  Token* tokens;
} LexerState;

void push_token(LexerState* state, TokenKind kind);

#define backup_start(state) (state)->start = (state)->index

#define set_token_data(state, field, value)\
  (state)->tokens[array_len((state)->tokens) - 1].data.field = (value)

#endif // RAIZ_LEXER_STATE_H
