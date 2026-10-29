#pragma once

#include "generics.h"
#include "symbols.h"

typedef struct {
  SymbolArray symbols;
} Scope;

typedef array_of(Scope) ScopeStack;

Symbol *scope_search(ScopeStack *stack, const char *name, size_t length);
Symbol *scope_insert(ScopeStack *stack, Span *span, size_t value_id);
Symbol *scope_search_or_insert(ScopeStack *stack, Span *span, size_t value_id);
Scope *scope_new(ScopeStack *stack);
