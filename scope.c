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

Symbol *Scope_search_single_level(Scope *sco, sv_t ident) {
  Symbol *sym;

  da_for(sym, &sco->symbols) {
    if (sv_equals(&sym->ident, &ident))
      return sym;
  }

  return NULL;
}

Symbol *Scope_search_until_global(Scope *sco, sv_t ident) {
  Symbol *sym;
  Scope *cur = sco;

  do {
    sym = Scope_search_single_level(cur, ident);
    if (sym)
      return sym;

    cur = cur->parent;
  } while (cur != NULL);

  return NULL;
}

Symbol *Scope_search_specific_kind_single_level(
    Scope *sco, SymbolKind kind, sv_t ident
) {
  Symbol *sym;

  da_for(sym, &sco->symbols) {
    if (sv_equals(&sym->ident, &ident) && sym->kind == kind)
      return sym;
  }

  return NULL;
}

Symbol *Scope_search_specific_kind_until_global(
    Scope *sco, SymbolKind kind, sv_t ident
) {
  Symbol *sym;
  Scope *cur = sco;

  do {
    sym = Scope_search_specific_kind_single_level(cur, kind, ident);
    if (sym)
      return sym;

    cur = cur->parent;
  } while (cur != NULL);

  return NULL;
}

void Scope_insert(Scope *sco, Symbol sym) {
  da_add(&sco->symbols, sym);
}

void Scope_insert_builtins(Scope *sco) {
  Symbol type_int = (Symbol){.kind = SYM_TYPE, .type = &g_TYPE_int};
  Symbol type_char = (Symbol){.kind = SYM_TYPE, .type = &g_TYPE_char};
  Symbol type_bool = (Symbol){.kind = SYM_TYPE, .type = &g_TYPE_bool};
  Symbol type_byte = (Symbol){.kind = SYM_TYPE, .type = &g_TYPE_byte};

  Scope_insert(sco, type_int);
  Scope_insert(sco, type_char);
  Scope_insert(sco, type_bool);
  Scope_insert(sco, type_byte);
}

void Scope_free(Scope *sco) {
  if (sco->inner != NULL)
    Scope_free(sco->inner);

  da_free(&sco->symbols);
  free(sco);
}

#endif // RAIZ_SCOPE_C
