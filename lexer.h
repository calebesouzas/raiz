#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

enum TokenFlags {
  TOKEN_FLAG_BREAKING = 1 << 0, // breaks Pratt Parser loop
  TOKEN_FLAG_OP       = 1 << 1, // is it an operator?
  TOKEN_FLAG_UNARY    = 1 << 2, // can it also be unary operator?
  TOKEN_FLAG_GROUPING = 1 << 3, // does it have a needed pair?
  TOKEN_FLAG_KEYWORD  = 1 << 4, // pretty obvious...
  TOKEN_FLAG_STARTER  = 1 << 5, // it may start an expression-like statement
  TOKEN_FLAG_FINISHER = 1 << 6, // can it finish an expression-like statement?
  TOKEN_FLAG_RIGHT    = 1 << 7, // is it a right associative infix operator?
  TOKEN_FLAG_CONST    = 1 << 8, // is it a constant?
};

#define TOKEN_X_MACRO\
  X(TOKEN_INVALID,  0)\
  X(TOKEN_NUMBER,   TOKEN_FLAG_CONST)\
  X(TOKEN_IDENT,    0)\
\
  X(TOKEN_EQUAL,      TOKEN_FLAG_OP | TOKEN_FLAG_RIGHT)\
  X(TOKEN_PLUS,       TOKEN_FLAG_OP)\
  X(TOKEN_MINUS,      TOKEN_FLAG_OP | TOKEN_FLAG_UNARY)\
  X(TOKEN_STAR,       TOKEN_FLAG_OP)\
  X(TOKEN_SLASH,      TOKEN_FLAG_OP)\
  X(TOKEN_EQUAL_X2,   TOKEN_FLAG_OP)\
  X(TOKEN_BANG,       TOKEN_FLAG_OP | TOKEN_FLAG_UNARY)\
  X(TOKEN_BANG_EQUAL, TOKEN_FLAG_OP)\
  X(TOKEN_AMPER,      TOKEN_FLAG_OP)\
  X(TOKEN_AMPER_X2,   TOKEN_FLAG_OP)\
  X(TOKEN_PIPE,       TOKEN_FLAG_OP)\
  X(TOKEN_PIPE_X2,    TOKEN_FLAG_OP)\
  X(TOKEN_HAT,        TOKEN_FLAG_OP)\
  X(TOKEN_TILDE,      TOKEN_FLAG_OP | TOKEN_FLAG_UNARY)\
  X(TOKEN_LESS,       TOKEN_FLAG_OP)\
  X(TOKEN_LESS_X2,    TOKEN_FLAG_OP)\
  X(TOKEN_LESS_EQUAL, TOKEN_FLAG_OP)\
  X(TOKEN_GREAT,      TOKEN_FLAG_OP)\
  X(TOKEN_GREAT_X2,   TOKEN_FLAG_OP)\
  X(TOKEN_GREAT_EQUAL,TOKEN_FLAG_OP)\
\
  X(TOKEN_L_PAREN, TOKEN_FLAG_GROUPING)\
  X(TOKEN_R_PAREN, TOKEN_FLAG_GROUPING | TOKEN_FLAG_BREAKING)\
  X(TOKEN_L_CURLY, TOKEN_FLAG_GROUPING)\
  X(TOKEN_R_CURLY, TOKEN_FLAG_GROUPING | TOKEN_FLAG_BREAKING)\
\
  X(TOKEN_VAR, TOKEN_FLAG_KEYWORD | TOKEN_FLAG_STARTER)\
  X(TOKEN_VAL, TOKEN_FLAG_KEYWORD | TOKEN_FLAG_STARTER)\
  X(TOKEN_TRUE, TOKEN_FLAG_KEYWORD | TOKEN_FLAG_CONST)\
  X(TOKEN_FALSE, TOKEN_FLAG_KEYWORD | TOKEN_FLAG_CONST)\
\
  X(TOKEN_NEWLINE, TOKEN_FLAG_BREAKING | TOKEN_FLAG_FINISHER)\
\
  X(TOKEN_EOF, TOKEN_FLAG_BREAKING)

enum TokenKind {
#define X(kind, flags) kind,
TOKEN_X_MACRO
#undef X
};

const int TOKEN_FLAGS[] = {
#define X(kind, flags) flags,
TOKEN_X_MACRO
#undef X
};

#define TOKEN_IDENTIFIER_SIZE ((size_t)32)

typedef struct {
  enum TokenKind kind;
  int flags;
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
  int value;
};

const struct TokenKeywordTable KEYWORDS[] = {
  {"var", 3, TOKEN_VAR, 0},
  {"val", 3, TOKEN_VAL, 0},
  {"true", 4, TOKEN_TRUE, 1},
  {"false", 5, TOKEN_FALSE, 0},
};

#endif // RAIZ_LEXER_H
