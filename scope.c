#ifndef RAIZ_SCOPE_C
#define RAIZ_SCOPE_C

Scope *Scope_(void) {
  Scope *s = malloc(sizeof(*s));
  if (s)
    memset(s, 0, sizeof(*s));
  return s;
}

Scope *Scope_new(Scope *parent) {
  Scope *inner = Scope_();
  inner->parent = parent;
  parent->inner = inner;
  return inner;
}

Symbol *Scope_search(Scope *sco, char *symbol, size_t len) {
  Symbol *sym;
  Scope *cur = sco;
  size_t l = len > 0 ? len : strlen(symbol);
  if (l > TOKEN_IDENTIFIER_SIZE) {
    fprintf(stderr, "error: symbol '%s' length (%zu) is greater than %zu\n",
            symbol, l, TOKEN_IDENTIFIER_SIZE);
    return NULL;
  }
  do {
    da_for(sym, &cur->symbols) {
      if (strcmp(sym->ident, symbol) == 0)
        return sym;
    }
    cur = cur->parent;
  } while (cur != NULL);
  return NULL;
}

Scope *Scope_next(Scope **sco) { (*sco) = (*sco)->parent; return *sco; }
Scope *Scope_prev(Scope **sco) { (*sco) = (*sco)->inner; return *sco; }

#endif // RAIZ_SCOPE_C
