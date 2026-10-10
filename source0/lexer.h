#pragma once

#include "libc.h"
#include "token.h"

typedef struct Lexer {
  const char *source;
  size_t size;

  const char *cursor;
  size_t remaining;
} Lexer;

Token next_token(Lexer *L);
