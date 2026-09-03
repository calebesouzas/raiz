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

typedef struct {
  int search_kind;
  int ignore_kind;
  size_t type_indirection;
} ScopeSearchOpts;

#define Scope_search_single_level(sco, ident, ...)\
  Scope_search_single_level_opt((sco), (ident),\
    ((ScopeSearchOpts){__VA_ARGS__}))

#define Scope_search_until_global(sco, ident, ...)\
  Scope_search_single_level_opt((sco), (ident),\
    ((ScopeSearchOpts){__VA_ARGS__}))

Symbol *Scope_search_single_level_opt(
    Scope *sco, sv_t ident, ScopeSearchOpts opts
) {
  Symbol *sym;

  da_for(sym, &sco->symbols) {
    if (sv_equals(&sym->ident, &ident)) {
      if (opts.search_kind != SYM_ANY && sym->kind != opts.search_kind) {
        continue;
      } else if (opts.ignore_kind != SYM_ANY && sym->kind == opts.ignore_kind) {
        continue;
      }
      if (opts.search_kind == SYM_TYPE && sym->kind == opts.search_kind) {
        if (sym->type->pattern.ptr_count == opts.type_indirection) {
          return sym;
        }
        continue;
      }
      return sym;
    }
  }

  return NULL;
}

Symbol *Scope_search_until_global_opt(
    Scope *sco, sv_t ident, ScopeSearchOpts opts
) {
  Symbol *sym;
  Scope *cur = sco;

  do {
    sym = Scope_search_single_level_opt(cur, ident, opts);
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
#define X(__name, __size)\
  Symbol type_ ## __name = (Symbol){\
    .kind = SYM_TYPE,\
    .ident = (g_TYPE_ ## __name).pattern.name,\
    .type = &g_TYPE_ ## __name,\
  };\
  Scope_insert(sco, type_ ## __name);

  TYPES_BUILTIN
#undef X
}

Symbol *Scope_search_or_insert(Scope *sco, sv_t ident, Symbol substitute) {
  Symbol *sym = Scope_search_until_global(sco, ident);
  if (sym != NULL) {
    return sym;
  }

  Scope_insert(sco, substitute);
  return &sco->symbols.dat[sco->symbols.len - 1];
}

Scope *Scope_find_global(Scope *sco) {
  Scope *cur;
  for (cur = sco; cur->parent != NULL; cur = sco->parent);
  return cur;
}

void Scope_free(Scope *sco) {
  if (sco->inner != NULL)
    Scope_free(sco->inner);

  da_free(&sco->symbols);
  free(sco);
}

#endif // RAIZ_SCOPE_C
