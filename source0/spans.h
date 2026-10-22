#pragma once

#include "libc.h"

typedef struct {
  const char *file_path;
  size_t line;
  size_t column;
  size_t index;
  const char *text;
  size_t length;
} Span;
