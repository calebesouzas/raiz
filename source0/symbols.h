#pragma once

#include "libc.h"
#include "ast.h"

typedef struct {
  Span span;
  bool poisoned : 1;
  struct Expr *value;
} Symbol;

#define token_to_symbol(token) ((Symbol){.span = (token)->span, .poisoned = false, .value = NULL,})
