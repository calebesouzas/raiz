#include "state.h"
#include "tokens.h"

#include "raiz_arrays.h"

void push_token(LexerState *state, TokenKind kind) {
  Token token = {
    .column = state->column,
    .line = state->line,
    .start = state->start,
    .kind = kind,
    .len = state->index - state->start,
    // '.data' remains unset
  };

  array_push(state->tokens, token);
}

