#pragma once

#include "libc.h"
#include "spans.h"

typedef struct {
  Span span;
  size_t value_id;
  bool poisoned : 1;
  uint8_t error_count : 7; // limit: 127
} Symbol;

typedef array_of(Symbol) SymbolArray;

#define token_to_symbol(token) ((Symbol){.span = (token)->span, .poisoned = false, .value = NULL,})
