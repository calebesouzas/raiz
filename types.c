#ifndef RAIZ_TYPES_C
#define RAIZ_TYPES_C

const Type *Type_find(struct Scope *sco, TypePattern pattern) {
  Scope *cur = sco;
  do {
    Symbol *sym;
    da_for(sym, &sco->symbols) {
      // hold on!
      if (sym->kind == SYM_TYPE
          && sym->type->pattern.ptr_count == pattern.ptr_count
          && sv_equals(&sym->type->pattern.name, &pattern.name))
        return sym->type;
    }
    cur = sco->parent;
  } while (cur != NULL);
  return NULL;
}

#endif /* RAIZ_TYPES_C */
