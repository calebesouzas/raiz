#ifndef RAIZ_TYPES_C
#define RAIZ_TYPES_C

const Type *Type_from_string(char *string, size_t len) {
  if ('@' == string[0]) {
    string++;
    len--;
  }

  for (size_t i = 0; i < sizeof(g_TYPES)/sizeof(g_TYPES[0]); i++) {
    const Type *type = g_TYPES[i];
    if (type->name.len == len && strncmp(type->name.str, string, len) == 0) {
      return type;
    }
  }

  return NULL;
}

#endif /* RAIZ_TYPES_C */
