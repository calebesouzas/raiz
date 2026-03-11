#ifndef RAIZ_LEXER_STATE_H
#define RAIZ_LEXER_STATE_H

#include "tokens.h"
#include "raiz_arrays.h"

typedef struct {
  unsigned int current, start, start_line, start_column, line, column;
  Token* tokens;
} LexerState;

#define raiz_init_lexer()\
  (LexerState) {\
    .column = 1,\
    .line = 1,\
    .current = 0,\
    .start = 0,\
    .start_line = 1,\
    .start_column = 1,\
    .tokens = NULL,\
  }

void push_token(LexerState* state, TokenKind kind);

#define backup_start(state)\
  do {\
    (state)->start = (state)->current;\
    (state)->start_line = (state)->line;\
    (state)->start_column = (state)->column;\
  } while (0)

uint update_state(LexerState* state, char current_char);

#define set_token_data(state, field, value)\
  (state)->tokens[array_len((state)->tokens) - 1].data.field = (value)

#endif // RAIZ_LEXER_STATE_H
