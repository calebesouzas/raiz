#ifndef RAIZ_TYPES_H
#define RAIZ_TYPES_H

struct Scope;

#define TYPES_BUILTIN\
  X(int, 4)\
  X(char, 1)\
  X(byte, 1)\
  X(bool, 1)

typedef struct {
  size_t ptr_count;
  sv_t name;
} TypePattern;

#define Type_pattern(__name, ...)\
  ((TypePattern){.name = sv_make(__name), __VA_ARGS__})

typedef struct Type {
  enum {
    #define X(typename, typesize) TYPE_ ## typename,
    TYPES_BUILTIN
    #undef X
  } kind;
  size_t size; // 0 for unknown
  TypePattern pattern;
} Type;

#define g_TYPE(typename, typesize)\
const Type g_TYPE_##typename = (Type){\
  .pattern = {\
    .name = {.ptr = #typename, .len = sizeof(#typename) - 1},\
    .ptr_count = 0,\
  },\
  .kind = TYPE_ ## typename,\
  .size = (typesize),\
};

#define X g_TYPE
TYPES_BUILTIN
#undef X

const Type *g_TYPES[] = {
  #define X(typename, typesize) &g_TYPE_ ## typename,
  TYPES_BUILTIN
  #undef X
};

const Type *Type_from_pattern(TypePattern pattern);

const Type *Type_find(struct Scope *sco, TypePattern pattern);

#endif // RAIZ_TYPES_H
