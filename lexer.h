#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

enum TokenFlags {
  TOKEN_FLAG_BREAKING = 1 << 16, // breaks Pratt Parser loop
  TOKEN_FLAG_OP       = 1 << 17, // is it an operator?
  TOKEN_FLAG_UNARY    = 1 << 18, // can it also be unary operator?
  TOKEN_FLAG_GROUPING = 1 << 19, // does it have a needed pair?
  TOKEN_FLAG_KEYWORD  = 1 << 20, // pretty obvious...
  TOKEN_FLAG_STARTER  = 1 << 21, // it may start an expression-like statement
  TOKEN_FLAG_FINISHER = 1 << 22, // can it finish an expression-like statement?
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
  TOKEN_VAR     = 8 | TOKEN_FLAG_KEYWORD | TOKEN_FLAG_STARTER,
  TOKEN_VAL     = 9 | TOKEN_FLAG_KEYWORD | TOKEN_FLAG_STARTER,
  TOKEN_IDENT   = 10,
  TOKEN_EQUAL   = 11 | TOKEN_FLAG_OP,
  TOKEN_NEWLINE = 12 | TOKEN_FLAG_BREAKING | TOKEN_FLAG_FINISHER,
  TOKEN_EOF     = 13 | TOKEN_FLAG_BREAKING,
};

#define TOKEN_IDENTIFIER_SIZE 32

typedef struct {
  enum TokenKind kind;
  int value; // if it's `TOKEN_NUMBER`
  char ident[TOKEN_IDENTIFIER_SIZE]; // if it's `TOKEN_IDENT`
} Token;
da_make(Token_A, Token*);

int Lexer_tokenize(Token_A *toks, char *source);

char *token_label(Token *tok);
bool token_keyword(Token *tok, char *ident, size_t len);

struct TokenKeywordTable {
  char *string;
  size_t len;
  uint32_t kind;
};

const struct TokenKeywordTable KEYWORDS[] = {
  {"var", 3, TOKEN_VAR},
  {"val", 3, TOKEN_VAL},
};

#endif // RAIZ_LEXER_H
