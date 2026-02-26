#ifndef RAIZ_ARRAY_UTILITY_H
#define RAIZ_ARRAY_UTILITY_H

typedef struct {
  unsigned int count;
  unsigned int capacity;
} ArrayHeader;

#define ARRAY_INIT_CAPACITY (256)

#define array_len(array) ((ArrayHeader*)(array) - 1)->count

#define array_push(array, value)\
  do {\
    if (array == NULL) {\
      ArrayHeader *header = malloc(sizeof(*array) * ARRAY_INIT_CAPACITY + sizeof(*header));\
      if (header != NULL) {\
        header->capacity = ARRAY_INIT_CAPACITY;\
        header->count = 0;\
        (array) = (void*)(header + 1);\
      }\
    }\
    ArrayHeader *header = (ArrayHeader*)(array) - 1;\
    if (header->count >= header->capacity) {\
      header->capacity *= 1.5;\
      header = realloc(header, sizeof(*array) *header->capacity + sizeof(*header));\
      (array) = (void*)(header + 1);\
    }\
    (array)[header->count++] = (value);\
  } while (0)

#define array_free(array) free((ArrayHeader*)(array) - 1)

#endif /* ifndef RAIZ_ARRAY_UTILITY_H */
