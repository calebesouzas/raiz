#include "tokens.h"
#include <stddef.h>

TokenKind is_reserved_keyword(char*const word, size_t len) {
  const char *token_strings[] = {
    #define DEF_TOK(variant, string) string,
    #include "tokens.def"
    #undef DEF_TOK
    NULL
  };


}
