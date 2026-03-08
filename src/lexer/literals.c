#include "tokens.h"
#include "state.h"

#include "raiz_strings.h"

void handle_string_literal(LexerState *state, char*const source_code) {
      backup_start(state);
      update_state(state, '"'); // consume '"'

      for (; source_code[state->index] && source_code[state->index] != '"';
             update_state(state, source_code[state->index]));

      push_token(state, RAIZ_TOKEN_LITERAL_STRING);

      // with 'i - 1' we ignore the closing '"'
      char* string = NULL;
      string_push_slice(string, source_code + state->start, 
                        state->index - state->start);
      set_token_data(state, s_val, string);
}
