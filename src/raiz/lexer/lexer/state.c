#include "state.h"
#include "tokens.h"

#include "raiz_arrays.h"

void push_token(LexerState *state, TokenKind kind) {
  Token token = {
    .column = state->start_column,
    .line = state->start_line,
    .start = state->start,
    .kind = kind,
    .len = state->current - state->start,
    // '.data' remains unset
  };

  array_push(state->tokens, token);
}

uint update_state(LexerState *state, char current_char) {
  switch (current_char) {
  case '\n':
    state->line++;
    state->column = 1;
    break;
  default:
    state->column++;
    break;
  }

  return state->current++; // increment and returns it self - 1
}
