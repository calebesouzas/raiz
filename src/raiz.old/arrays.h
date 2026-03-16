#ifndef RAIZ_ARRAY_H
#define RAIZ_ARRAY_H

#ifndef RAIZ_DEBUG_H
#include "debug.h"
#endif

typedef struct {
  size_t count;
  size_t capacity;
} Raiz_ArrayHeader;

#ifndef RAIZ_ARRAY_INIT_CAPACITY
#define RAIZ_ARRAY_INIT_CAPACITY (256)
#endif

#define raiz_array_len(array) ((Raiz_ArrayHeader*)(array) - 1)->count

#define raiz_array_push(array, value)\
  do {\
    if (array == NULL) {\
      Raiz_ArrayHeader *header = malloc(\
        sizeof(*array) * RAIZ_ARRAY_INIT_CAPACITY + sizeof(*header)\
      );\
      RAIZ_LOG(\
        "init '%s' with %u bytes",\
        RAIZ_STRING_VAR_NAME(array),\
        sizeof(*array) * RAIZ_ARRAY_INIT_CAPACITY + sizeof(*header)\
      );\
      if (header != NULL) {\
        header->capacity = RAIZ_ARRAY_INIT_CAPACITY;\
        header->count = 0;\
        (array) = (void*)(header + 1);\
      }\
    }\
    Raiz_ArrayHeader *header = (Raiz_ArrayHeader*)(array) - 1;\
    if (header->count >= header->capacity) {\
      header->capacity *= 1.5;\
      header = realloc(\
        header, sizeof(*array) *header->capacity + sizeof(*header)\
      );\
      RAIZ_LOG(\
        "realloc '%s' with %u bytes",\
        RAIZ_STRING_VAR_NAME(array),\
        sizeof(*array) *header->capacity + sizeof(*header)\
      );\
      (array) = (void*)(header + 1);\
    }\
    (array)[header->count++] = (value);\
  } while (0)

#define raiz_array_free(array) free((Raiz_ArrayHeader*)(array) - 1)

#endif // RAIZ_ARRAY_H
