#ifndef RAIZ_TYPES_H
#define RAIZ_TYPES_H

#define TYPES_BUILTIN\
  X(int, 4)\
  X(uint, 4)\
  X(char, 1)\
  X(byte, 1)\
  X(bool, 1)\
  X(string, 0)

typedef struct Type {
  struct {
    char *str;
    size_t len;
  } name;
  enum {
    #define X(typename, typesize) TYPE_ ## typename,
    TYPES_BUILTIN
    #undef X
  } kind;
  size_t size; // 0 for unknown
} Type;

#define g_TYPE(typename, typesize)\
const Type g_TYPE_##typename = (Type){\
  .name = {.str = #typename, .len = sizeof(#typename)},\
  .kind = TYPE_ ## typename,\
  .size = (typesize),\
};

#define X g_TYPE
TYPES_BUILTIN
#undef X

#endif // RAIZ_TYPES_H
