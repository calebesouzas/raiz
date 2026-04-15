#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

#include "values.h"

typedef enum
{
  RZ_TOKEN_ERROR,
  RZ_TOKEN_EOF,
  RZ_TOKEN_LITERAL,
  RZ_TOKEN_IDENT,
  RZ_TOKEN_OP,
  RZ_TOKEN_LPAREN,
  RZ_TOKEN_RPAREN,
} Rz_TokenKind;

union Rz_TokenData
{
  Rz_Operator op;
  Rz_Value literal;
};

typedef struct
{
  Rz_TokenKind kind;
  union Rz_TokenData as;
  const char *lexeme;
  size_t len;
} Rz_Token;

typedef struct
{
  const char *source;
  size_t start, current, source_len;
} Rz_Lexer;

Rz_Lexer rz_lexer_new(const char *source);
Rz_Token rz_lexer_next_token(Rz_Lexer *lex);

#endif // RAIZ_LEXER_H
