#pragma once

#include "libc.h"
#include "token.h"

typedef struct Lexer {
  const char *source;
  size_t source_length;

  const char *cursor;
  size_t remaining;

  size_t column_count;
  size_t line_count;
  const char *file_path;
} Lexer;

typedef enum {
  LEXERFLAG_ENABLE_NEW_LINE = 1 << 0,
} LexerFlags;

#define LEXER_DEFAULT_FLAGS (LEXERFLAG_ENABLE_NEW_LINE)

Lexer lexer_init(const char *source, size_t length, const char *file_path);

Token next_token(Lexer *L, uint32_t flags);
