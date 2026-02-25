#ifndef RAIZ_ARRAY_UTILITY_H
#define RAIZ_ARRAY_UTILITY_H

#define RZ_DEF_ARRAY_OF(type)\
  typedef struct {\
    type *elements;\
    uint32_t len;\
    uint32_t capacity;\
  } Array_##type

#endif /* ifndef RAIZ_ARRAY_UTILITY_H */
