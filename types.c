#ifndef RAIZ_TYPES_C
#define RAIZ_TYPES_C

const Type *Type_from_string(char *string, size_t len) {
  if ('@' == string[0]) {
    string++;
  }

  #define X(__type, __size)\
    if (sizeof(#__type) == len && strncmp(#__type, string, len) == 0) {\
      return &g_TYPE_ ## __type;\
    }
  TYPES_BUILTIN
  #undef X

  return NULL;
}

#endif /* RAIZ_TYPES_C */
