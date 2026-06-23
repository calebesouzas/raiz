#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

enum TokenFlags {
  TOKEN_FLAG_BREAKING = 1 << 16,
  TOKEN_FLAG_OP       = 1 << 17,
  TOKEN_FLAG_UNARY    = 1 << 18,
  TOKEN_FLAG_GROUPING = 1 << 19,
};

enum TokenKind {
  TOKEN_INVALID = 0,
  TOKEN_NUMBER  = 1,
  TOKEN_PLUS    = 2 | TOKEN_FLAG_OP,
  TOKEN_MINUS   = 3 | TOKEN_FLAG_OP | TOKEN_FLAG_UNARY,
  TOKEN_STAR    = 4 | TOKEN_FLAG_OP,
  TOKEN_SLASH   = 5 | TOKEN_FLAG_OP,
  TOKEN_L_PAREN = 6 | TOKEN_FLAG_GROUPING,
  TOKEN_R_PAREN = 7 | TOKEN_FLAG_GROUPING | TOKEN_FLAG_BREAKING,
  TOKEN_EOF     = 8 | TOKEN_FLAG_BREAKING,
};

typedef struct {
  uint32_t kind;
  int value; // if it's `TOKEN_NUMBER`
} Token;
da_make(Token_A, Token*);

int Lexer_tokenize(Token_A *toks, char *source);

char *token_label(Token *tok);

#endif // RAIZ_LEXER_H
