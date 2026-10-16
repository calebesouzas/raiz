#pragma once

#define array_of(ItemType) struct { ItemType *data; size_t count, capacity; }

#define array_size(array) (sizeof((array)->data[0]) * (array)->capacity)

#define array_init(array)\
do\
{\
  (array)->capacity = ARRAY_INITIAL_CAPACITY;\
  (array)->data = malloc(array_size(array));\
} while (0)

#define array_reserve(array, amount)\
do\
{\
  while ((array)->capacity < amount)\
  {\
    (array)->capacity *= 2;\
  }\
  (array)->data = realloc((array)->data, array_size(array));\
} while (0)

#define array_push(array, value)\
do\
{\
  array_reserve(array, (array)->count + 1);\
  if ((array)->data != NULL)\
  {\
    (array)->data[(array)->count++] = (value);\
  }\
} while (0)

#define array_free(array)\
do\
{\
  if ((array)->data != NULL)\
  {\
    free((array)->data);\
  }\
  (array)->count = 0;\
  (array)->capacity = 0;\
} while (0)

#define array_for(Type, iterator, array)\
for (Type *iterator = (array)->data;\
    iterator != NULL && iterator < (array)->data + (array)->count; iterator++)
