#pragma once

#include "libc.h"

typedef enum TokenType {
  TOKEN_END_OF_FILE,
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_LINE_BREAK,
} TokenType;

typedef struct {
  TokenType type;
  size_t length;
  const char *lexeme;
  union {
    int literal;
  } as;
} Token;

typedef struct TokenArena {
  Token *tokens;
  size_t count;
  struct TokenArena *next;
} TokenArena;

TokenArena *new_token_arena(void);
void free_token_arena(TokenArena *arena);
Token *new_token(TokenArena *arena);

bool token_is_operator(Token *token);
