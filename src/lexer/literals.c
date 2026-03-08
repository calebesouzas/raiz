#include "tokens.h"
#include "state.h"

#include "raiz_arrays.h"
#include "raiz_strings.h"

void handle_string_literal(LexerState *state, char*const source_code) {
      state->index++; // consume '"'
      backup_start(state);

      for (; source_code[state->index] && source_code[state->index] != '"';
             ++state->index);
      Token token;
      token.kind = RAIZ_TOKEN_LITERAL_STRING;
      token.len = state->index - state->start;
      token.column = state->column;
      token.line = state->line;
      token.start = state->start;

      // with 'i - 1' we ignore the closing '"'
      char* string = NULL;
      string_push_slice(string, source_code + state->start, 
                        state->index - state->start);
      token.data.s_val = string;

      array_push(state->tokens, token);
}
