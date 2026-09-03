#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

// crazy hack to get flag bit masks for free
#define TOKEN_FLAG_LIST_X_MACRO\
  X(BREAKING)\
  X(OPERATOR)\
  X(UNARY)\
  X(GROUPING)\
  X(KEYWORD)\
  X(STARTER)\
  X(FINISHER)\
  X(RIGHT_ASSOCIATIVE)\
  X(CONSTANT)\
  X(FUNCTION)\
  X(TYPE_FINISHER)\
  X(DECLARATOR)

enum {
#define X(__variant) _TOKEN_FLAG_ ## __variant,
  TOKEN_FLAG_LIST_X_MACRO
#undef X
};

enum TokenFlags {
#define X(__variant) TOKEN_FLAG_ ## __variant = 1 << _TOKEN_FLAG_ ## __variant,
  TOKEN_FLAG_LIST_X_MACRO
#undef X
};

#define TOKEN_X_MACRO\
  X(TOKEN_INVALID, 0)\
  X(TOKEN_NUMBER,  TOKEN_FLAG_CONSTANT)\
  X(TOKEN_IDENT,   TOKEN_FLAG_STARTER|TOKEN_FLAG_TYPE_FINISHER)\
\
  X(TOKEN_EQUAL,       TOKEN_FLAG_OPERATOR|TOKEN_FLAG_RIGHT_ASSOCIATIVE)\
  X(TOKEN_PLUS,        TOKEN_FLAG_OPERATOR)\
  X(TOKEN_MINUS,       TOKEN_FLAG_OPERATOR|TOKEN_FLAG_UNARY)\
  X(TOKEN_STAR,        TOKEN_FLAG_OPERATOR|TOKEN_FLAG_UNARY)\
  X(TOKEN_SLASH,       TOKEN_FLAG_OPERATOR)\
  X(TOKEN_EQUAL_X2,    TOKEN_FLAG_OPERATOR)\
  X(TOKEN_BANG,        TOKEN_FLAG_OPERATOR|TOKEN_FLAG_UNARY)\
  X(TOKEN_BANG_EQUAL,  TOKEN_FLAG_OPERATOR)\
  X(TOKEN_AMPER,       TOKEN_FLAG_OPERATOR|TOKEN_FLAG_UNARY)\
  X(TOKEN_AMPER_X2,    TOKEN_FLAG_OPERATOR)\
  X(TOKEN_PIPE,        TOKEN_FLAG_OPERATOR)\
  X(TOKEN_PIPE_X2,     TOKEN_FLAG_OPERATOR)\
  X(TOKEN_HAT,         TOKEN_FLAG_OPERATOR)\
  X(TOKEN_TILDE,       TOKEN_FLAG_OPERATOR|TOKEN_FLAG_UNARY)\
  X(TOKEN_LESS,        TOKEN_FLAG_OPERATOR)\
  X(TOKEN_LESS_X2,     TOKEN_FLAG_OPERATOR)\
  X(TOKEN_LESS_EQUAL,  TOKEN_FLAG_OPERATOR)\
  X(TOKEN_GREAT,       TOKEN_FLAG_OPERATOR)\
  X(TOKEN_GREAT_X2,    TOKEN_FLAG_OPERATOR)\
  X(TOKEN_GREAT_EQUAL, TOKEN_FLAG_OPERATOR)\
  X(TOKEN_PERCENT,     TOKEN_FLAG_OPERATOR)\
\
  X(TOKEN_L_PAREN, TOKEN_FLAG_GROUPING)\
  X(TOKEN_R_PAREN, TOKEN_FLAG_GROUPING|TOKEN_FLAG_BREAKING)\
  X(TOKEN_L_CURLY, TOKEN_FLAG_GROUPING|TOKEN_FLAG_BREAKING)\
  X(TOKEN_R_CURLY, TOKEN_FLAG_GROUPING|TOKEN_FLAG_BREAKING)\
\
  X(TOKEN_AT,     0)\
  X(TOKEN_COLLON, 0)\
\
  X(TOKEN_VAR, TOKEN_FLAG_KEYWORD|TOKEN_FLAG_DECLARATOR)\
  X(TOKEN_VAL, TOKEN_FLAG_KEYWORD|TOKEN_FLAG_DECLARATOR)\
\
  X(TOKEN_TRUE,     TOKEN_FLAG_KEYWORD|TOKEN_FLAG_CONSTANT)\
  X(TOKEN_FALSE,    TOKEN_FLAG_KEYWORD|TOKEN_FLAG_CONSTANT)\
  X(TOKEN_IF,       TOKEN_FLAG_KEYWORD)\
  X(TOKEN_ELSE,     TOKEN_FLAG_KEYWORD|TOKEN_FLAG_BREAKING)\
  X(TOKEN_THEN,     TOKEN_FLAG_KEYWORD|TOKEN_FLAG_BREAKING)\
  X(TOKEN_WHILE,    TOKEN_FLAG_KEYWORD|TOKEN_FLAG_STARTER)\
  X(TOKEN_BREAK,    TOKEN_FLAG_KEYWORD|TOKEN_FLAG_STARTER|TOKEN_FLAG_BREAKING)\
  X(TOKEN_CONTINUE, TOKEN_FLAG_KEYWORD|TOKEN_FLAG_STARTER|TOKEN_FLAG_BREAKING)\
\
  X(TOKEN_PRINT, TOKEN_FLAG_KEYWORD|TOKEN_FLAG_FUNCTION)\
  X(TOKEN_READ,  TOKEN_FLAG_KEYWORD|TOKEN_FLAG_FUNCTION)\
\
  X(TOKEN_NEWLINE, TOKEN_FLAG_BREAKING|TOKEN_FLAG_FINISHER)\
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
  // identification
  enum TokenKind kind;
  int flags;

  // data
  Value literal;

  // metadata
  char *lexeme;
  size_t start, line, len;
} Token;
da_make(Token_A, Token*);

#define Token_fmt(tok) (tok)->len, (tok)->lexeme

typedef struct {
  Token_A *toks;
  char *source;
  size_t source_len;

  char c;
  size_t i;

  // For `Token` metadata
  size_t start, lines;
} Lexer;

Lexer Lexer_setup(Token_A *toks, char *source, size_t len);
int Lexer_tokenize(Lexer *lex);

char *token_label(Token *tok);
bool token_keyword(Token *tok, char *ident, size_t len);
size_t Token_distance(Token *from, Token *to);
#define token_sv(tok) ((sv_t){.ptr = (tok)->lexeme, .len = (tok)->len})

struct TokenKeywordTable {
  char *string;
  size_t len;
  uint32_t kind;
  Value value;
};

const struct TokenKeywordTable KEYWORDS[] = {
  {"var", 3, TOKEN_VAR},
  {"val", 3, TOKEN_VAL},
  {"true", 4, TOKEN_TRUE, Value_(&g_TYPE_bool, bool, 1)},
  {"false", 5, TOKEN_FALSE, Value_(&g_TYPE_bool, bool, 0)},
  {"if", 2, TOKEN_IF},
  {"else", 4, TOKEN_ELSE},
  {"while", 5, TOKEN_WHILE},
  {"then", 4, TOKEN_THEN},
  {"break", 5, TOKEN_BREAK},
  {"continue", 8, TOKEN_CONTINUE},
  {"print", 5, TOKEN_PRINT},
  {"read", 4, TOKEN_READ},
};

Token Lexer_ident(Lexer *lex);

char Lexer_peek(Lexer *lex);
char Lexer_next(Lexer *lex);
char Lexer_advance(Lexer *lex);
char Lexer_cur(Lexer *lex);
bool Lexer_active(Lexer *lex);
char *Lexer_point(Lexer *lex);
size_t Lexer_len(Lexer *lex);
void Lexer_add(Lexer *lex, Token tok);

#endif // RAIZ_LEXER_H
