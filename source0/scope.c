#include "libc.h"
#include "scope.h"

Symbol *scope_search(ScopeStack *stack, const char *name, size_t length)
{
  Scope *scope = array_last(stack);
  do
  {
    array_for(Symbol, symbol, &scope->symbols)
    {
      if (symbol->span.length == length && strncmp(symbol->span.text, name, length) == 0)
      {
        return symbol;
      }
    }
    scope = scope > stack->data && scope < stack->data + stack->count ? scope - 1 : NULL;
  } while (scope != NULL);
  return NULL;
}

Symbol *scope_insert(ScopeStack *stack, Span *span, size_t value_id)
{
  Symbol symbol = {0};
  symbol.value_id = value_id;
  symbol.span = *span;
  Scope *scope = array_last(stack);
  array_push(&scope->symbols, symbol);
  return array_last(&scope->symbols);
}

Symbol *scope_search_or_insert(ScopeStack *stack, Span *span, size_t value_id)
{
  Symbol *symbol = scope_search(stack, span->text, span->length);
  if (symbol == NULL)
  {
    symbol = scope_insert(stack, span, value_id);
  }
  return symbol;
}

Scope *scope_new(ScopeStack *stack)
{
  Scope empty = {0};
  array_push(stack, empty);
  return array_last(stack);
}
