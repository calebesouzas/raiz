#pragma once

#include "libc.h"

typedef struct {
  Span span;
  bool poisoned : 1;
  size_t value_id;
} Symbol;

typedef array_of(Symbol) SymbolArray;

#define token_to_symbol(token) ((Symbol){.span = (token)->span, .poisoned = false, .value = NULL,})
