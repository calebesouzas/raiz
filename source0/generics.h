#pragma once

#include "libc.h"

#define array_of(ItemType) struct { ItemType *data; size_t count, capacity; }

#define array_size(array) (sizeof((array)->data[0]) * (array)->capacity)

#define array_check(array)\
do\
{\
  if ((array)->data == NULL)\
  {\
    fprintf(stderr, "%s:%d: array allocation failed\n", __FILE__, __LINE__);\
    abort();\
  }\
} while (0)

#define ARRAY_INITIAL_CAPACITY 16
#define array_init(array, initial_capacity)\
do\
{\
  (array)->capacity = (initial_capacity) > 0? (initial_capacity) : ARRAY_INITIAL_CAPACITY;\
  (array)->data = malloc(array_size(array));\
  array_check(array);\
} while (0)

#define array_reserve(array, amount)\
do\
{\
  while ((array)->capacity < amount)\
  {\
    (array)->capacity *= 2;\
  }\
  (array)->data = realloc((array)->data, array_size(array));\
  array_check(array);\
} while (0)

#define array_push(array, value)\
do\
{\
  if ((array)->capacity == 0)\
  {\
    array_init(array, 0);\
  }\
  else\
  {\
    array_reserve(array, (array)->count + 1);\
  }\
  if ((array)->data != NULL)\
  {\
    (array)->data[(array)->count++] = (value);\
  }\
} while (0)

#define array_last(array) (&(array)->data[(array)->count - 1])

#define array_free(array)\
do\
{\
  if ((array)->data != NULL)\
  {\
    free((array)->data);\
  }\
  (array)->data = NULL;\
  (array)->count = 0;\
  (array)->capacity = 0;\
} while (0)

#define array_for(Type, iterator, array)\
for (Type *iterator = (array)->data;\
    iterator != NULL && iterator < (array)->data + (array)->count; iterator++)
