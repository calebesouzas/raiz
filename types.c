#ifndef RAIZ_TYPES_C
#define RAIZ_TYPES_C

struct {
  Type **dat;
  size_t len, cap;
} Type_Pool = {0};

Type *Type_alloc(TypePattern pattern) {
  Type *type = malloc(sizeof(Type));
  assert(type != NULL && "buy more RAM lol");
  *type = (Type){.kind = TYPE_ptr, .pattern = pattern};
  da_add(&Type_Pool, type);
  return Type_Pool.dat[Type_Pool.len - 1];
}

bool Type_is_ptr(Type *type) {
  return type->pattern.ptr_count > 0;
}

Type *Type_find(struct Scope *sco, TypePattern pattern) {
  Symbol *sym = Scope_search_until_global(sco, pattern.name,
    .search_kind = SYM_TYPE, .type_indirection = pattern.ptr_count);

  if (sym) {
    assert(sym->type != NULL);
    return sym->type;
  }

  if (pattern.ptr_count > 0) {
    TypePattern retry_pattern = {.name = pattern.name, .ptr_count = 0};
    Type *retry_type = Type_find(sco, retry_pattern);

    if (retry_type) { // type without any indirection exists
      // we can implicitly declare it on demand now
      Scope *global = Scope_find_global(sco);
      Type *new_type = Type_alloc(pattern);
      Scope_insert(global, (Symbol){.kind = SYM_TYPE, .type = new_type});
      return new_type;
    }
  }
  return NULL;
}

#endif /* RAIZ_TYPES_C */
