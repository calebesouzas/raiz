#include "keywords.h"
#include "state.h"

#include "raiz_strings.h"

// NOTE: could implement a hash table instead of iterating through
// each keyword and comparing it with 'word'. Or a switch case at word's
// length and iterate over the keywords with the same length only
void set_if_is_keyword(LexerState *state, char*const word) {
 // i know there is a better way, but i don't know this way (not yet)
  unsigned int keywords_start = (unsigned int) RAIZ_TOKEN_LITERAL_STRING + 1;

  unsigned int len = state->index - state->start;

  for (unsigned int i = 0; 
      i < (sizeof(raiz_keywords) / sizeof(raiz_keywords[0]));
      ++i) {
    if (strncmp(word, raiz_keywords[i], len) == 0) {
      push_token(state, i + keywords_start); // hope it does work...
      return;
    }
  }

  push_token(state, RAIZ_TOKEN_IDENTIFIER);
  char* identifier = NULL;
  string_push_slice(identifier, word, len);
  set_token_data(state, s_val, identifier);
}
