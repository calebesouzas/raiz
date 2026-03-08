#ifndef RAIZ_LEXER_TOKENS_H
#define RAIZ_LEXER_TOKENS_H

// TODO: add the remaining comments for each TOKEN_<VARIANT>
typedef enum {
  #define RAIZ_DEFINE_TOKEN(variant, string) variant,
  #include "tokens.def"
  #undef RAIZ_DEFINE_TOKEN
  RAIZ_TOKEN_COUNT
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

#endif // RAIZ_LEXER_TOKENS_H
