#ifndef RAIZ_LEXER_HELPER_H
#define RAIZ_LEXER_HELPER_H

typedef unsigned int uint; // alias because it's too long

#define is_next(character) (source_code[(state->index) + 1] == character)

#define is_number(c) (c >= '0' && c <= '9')
#define is_alpha(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

#define is_identstart(c) (is_alpha(c) || c == '_' )
#define is_identchar(c) (is_identstart(c) || is_number(c))

#define set(kind) setn(kind, 1)
#define setn(kind, len)\
  (*token) = (Token){\
    kind,\
    len,\
    state->index\
  };\
  if (len > 1) (state->index) += len - 1;

#define CASE(value, code) case value: do { code } while (0); break;

#endif // RAIZ_LEXER_HELPER_H
