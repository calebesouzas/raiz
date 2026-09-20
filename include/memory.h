#ifndef RAIZ_MEMORY_H
#define RAIZ_MEMORY_H

#define make_allocator(typename)\
  typename *typename ## _(void) {\
    typename *v = malloc(sizeof(*v));\
    if (v != NULL) memset(v, 0, sizeof(*v));\
    return v;\
  }

#endif /* RAIZ_MEMORY_H */
