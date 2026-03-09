// #include "raiz_debug/logs.h"
#include "./helper.h" // CASE (auto break) macro

#include "raiz_arrays.h"

#include "tokens.h"
#include "state.h"

#include <stdio.h>

#include "simple_tokens.c"
#include "keywords.c"
#include "state.c"
#include "literals.c"

#ifdef RAIZ_DEBUG
void debug_added_token(LexerState* state);
#endif

Token* raiz_tokenize(char* const source_code) {
  LexerState state;
  for (state = raiz_init_lexer();
      source_code[state.current]; update_state(&state, source_code[state.current]))
  {
    #ifdef RAIZ_DEBUG
    short add = 1;
    #endif

    switch (source_code[state.current]) {
    case '\n':
    case ' ':
    case '\t':
    case '\r':
      #ifdef RAIZ_DEBUG
      add = 0;
      #endif
      break;
    case '"':
      handle_string_literal(&state, source_code);

      if (source_code[state.current] != '"') {
        fprintf(stderr, "raiz: expected '\"' closing string at %u:%u\n",
                state.line, state.column);
        array_free(state.tokens);
        return NULL;
      }
      break;
    default:
      if (handle_simple_token(&state, source_code)) {}
      // NOTE: only 32 bit integer numbers are handled
      // and there are no prefixes or suffixes
      // (such as '0x', '0b' prefixes or 'u' and 'f' suffixes)
      else if (is_number(source_code[state.current])) {
        // TODO: handle floating numbers
        int number = 0;
        backup_start(&state);

        // if entered this 'if' block, will run the loop below at least once
        // Then, the number and it's length are calculated correctly
        for (; is_number(source_code[state.current]);
            update_state(&state, source_code[state.current])) {
          // TODO: check for and skip '_'s for better reading of the number
          number = (number * 10) + (source_code[state.current] - '0');
        }

        push_token(&state, RAIZ_TOKEN_LITERAL_INT);
        
        set_token_data(&state, i_val, number);

        // don't skip chars after numbers!
        state.current--;
        state.column--;
      } else if (is_identstart(source_code[state.current])) {
        backup_start(&state);

        // get identifier length
        for (; is_identchar(source_code[state.current]);
            update_state(&state, source_code[state.current]));

        // NOTE: i could implement a hash map for performance and DX reasons
        set_if_is_keyword(&state, source_code + state.start);

        state.current--;
        state.column--;
      } else {
        fprintf(
          stderr,
          "raiz: invalid character at %u:%u: '%c'\n",
          state.line,
          state.column + 1,
          source_code[state.current]
        );
        array_free(state.tokens);
        return NULL;
      }
      break; // default case
    } // switch (source_code[i]) // current character

    #ifdef RAIZ_DEBUG
    if (add) debug_added_token(&state);
    #endif // ifdef RAIZ_DEBUG
  } // for (... source_code ...) // main loop

  return state.tokens;
}

void debug_added_token(LexerState* state) {
  const char* token_variant_names[] = {
    #define RAIZ_DEFINE_TOKEN(variant, string) #variant,
    #include "tokens.def"
    #undef RAIZ_DEFINE_TOKEN
    NULL
  };
  Token token = state->tokens[array_len(state->tokens) - 1];
  printf("[RAIZ]: add token [%u:%u:%u] (%s)",
      token.line, token.column, token.len,
      token_variant_names[token.kind]);

  switch (token.kind) {
  case RAIZ_TOKEN_LITERAL_CHAR:
    printf(" < '%s' >\n", token.data.c_val);
    break;
  case RAIZ_TOKEN_LITERAL_INT:
    printf(" < %d >\n", token.data.i_val);
    break;
  case RAIZ_TOKEN_LITERAL_STRING:
    printf(" < \"%s\" >\n", token.data.s_val);
    break;
  case RAIZ_TOKEN_IDENTIFIER:
    printf(" < %s >\n", token.data.s_val);
    break;
  default:
    printf("\n");
    break;
  }
}
