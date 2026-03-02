#ifndef RAIZ_TOKENS_H
#define RAIZ_TOKENS_H

// TODO: add the remaining comments for each TOKEN_<VARIANT>
typedef enum {
#define DEF_TOK(variant, string) variant,
#include "tokens.def"
#undef DEF_TOK
  RZ_TK_COUNT
} TokenKind;

typedef struct {
  TokenKind kind;
  unsigned int len, start, line, column;
  union {
    char *s_val;
    char c_val[4];
    int i_val;
    double f_val;
  } data;
} Token;

#endif /* RAIZ_TOKENS_H */
