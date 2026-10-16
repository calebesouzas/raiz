#pragma once

#include "libc.h"
#include "token.h"

typedef struct Lexer {
  const char *source;
  size_t size;

  const char *cursor;
  size_t remaining;

  size_t column_count;
  size_t line_count;
} Lexer;

Token next_token(Lexer *L);
