#pragma once

#include "libc.h"

#include "spans.h"

typedef enum TokenType {
  TOKEN_END_OF_FILE = 0, // do not assign to the other variants!
  TOKEN_ERROR,
  TOKEN_NUMBER,
  TOKEN_IDENTIFIER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_EQUAL,
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_LINE_BREAK,
  TOKEN_OPEN_CURLY,
  TOKEN_CLOSE_CURLY,
  __TOKEN_COUNT, // keep this one as the last!
} TokenType;

typedef struct {
  TokenType type;
  Span span;
  union {
    int literal;
    struct {
      const char *message;
    } error;
  } as;
} Token;

#define __TOKEN_FMT(token)\
((token)->span.length > INT_MAX ? INT_MAX : (int)(token)->span.length), (token)->span.text

typedef enum {
  OPERATOR_INFIX,
  OPERATOR_PREFIX,
} OperatorPrecedenceType;

typedef struct TokenArena {
  Token *tokens;
  size_t count;
  struct TokenArena *next;
} TokenArena;

TokenArena *new_token_arena(void);
void free_token_arena(TokenArena *arena);
Token *new_token(TokenArena *arena);

bool token_is_operator(Token *token);
uint8_t operator_precedence(Token *token, OperatorPrecedenceType type);
const char *token_name(TokenType type);
