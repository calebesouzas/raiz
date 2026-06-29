#ifndef RAIZ_SCOPE_C
#define RAIZ_SCOPE_C

Scope *Scope_(void) {
  Scope *s = malloc(sizeof(*s));
  if (s)
    memset(s, 0, sizeof(*s));
  return s;
}

Scope *Scope_copy(Scope *parent) {
  Scope *s = malloc(sizeof(*s));
  if (s) {
    da_copy(&s->symbols, &parent->symbols);

    if (parent->inner) {
      Scope *i = Scope_copy(parent->inner);
      s->inner = i;
    }
  }

  return s;
}

Scope *Scope_new(Scope *parent) {
  Scope *inner = Scope_();
  inner->parent = parent;
  parent->inner = inner;
  return inner;
}

Symbol *Scope_search_single_level(Scope *sco, char *symbol, size_t len) {
  Symbol *sym;

  da_for(sym, &sco->symbols) {
    if (strncmp(sym->ident, symbol, len) == 0)
      return sym;
  }

  return NULL;
}

Symbol *Scope_search_until_global(Scope *sco, char *symbol, size_t len) {
  Symbol *sym;
  Scope *cur = sco;

  do {
    sym = Scope_search_single_level(cur, symbol, len);
    if (sym)
      return sym;

    cur = cur->parent;
  } while (cur != NULL);

  return NULL;
}

void Scope_free(Scope *sco) {
  if (sco->inner != NULL)
    Scope_free(sco->inner);

  da_free(&sco->symbols);
  free(sco);
}
#endif // RAIZ_SCOPE_C
