#ifndef RAIZ_VALUE_H
#define RAIZ_VALUE_H

typedef struct {
  uint64_t data;
  const Type *type;
} Value;

#define Value_(__variant, __type, __value)\
  ((Value){.type = (__variant), .data = ((__type) __value)})

#endif // RAIZ_VALUE_C
