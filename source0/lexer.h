#pragma once

#include "libc.h"

typedef struct Lexer {
  const char *source;
  size_t size;

  const char *cursor;
  size_t remaining;
} Lexer;

typedef enum TokenType {
  TOKEN_END_OF_FILE,
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_LINE_BREAK,
} TokenType;

typedef struct Token {
  TokenType type;
  size_t length;
  const char *lexeme;
  union {
    int literal;
  } as;
} Token;
