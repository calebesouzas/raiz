#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

// TODO: add the remaining comments for each TOKEN_<VARIANT>
typedef enum {
  #define RAIZ_DEFINE_TOKEN(variant, string) variant,
  #include "tokens.def"
  #undef RAIZ_DEFINE_TOKEN
  RAIZ_TOKEN_COUNT
} Raiz_TokenKind;

typedef struct {
  Raiz_TokenKind kind;
  size_t len, start, line, column;
  union {
    char *s_val;
    char c_val; // we'll use only ASCII for now
    int i_val;
    double d_val; // TODO: add support for floating number literals
  } data;
} Raiz_Token;

typedef struct {
  size_t current, start, start_line, start_column, line, column;
  Raiz_Token* tokens; // dynamic array
} Raiz_LexerState;

Raiz_Token* raiz_tokenize(char* const source_code);

#endif // RAIZ_LEXER_TOKENS_H
