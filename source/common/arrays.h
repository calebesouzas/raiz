#pragma once

#define RAIZ_DA_INITIAL_CAPACITY 256

#define raiz_da_append(da, value)\
  do\
  {\
    if ((da)->capacity == 0)\
    {\
      (da)->capacity = RAIZ_DA_INITIAL_CAPACITY;\
      (da)->items = malloc(sizeof(*(da)->items)*(da)->capacity);\
    }\
    if ((da)->count >= (da)->capacity)\
    {\
      (da)->capacity *= 2;\
      (da)->items = realloc((da)->items, sizeof(*(da)->items)*(da)->capacity);\
    }\
    if ((da)->items != NULL)\
    {\
      (da)->items[(da)->count++] = (value);\
    }\
  } while (0)

#define raiz_da_free_malloced_items(da)\
  do\
  {\
    for (size_t i = 0; i < (da)->count; i++)\
    {\
      free((da)->items[i]);\
    }\
  } while (0)

#define RAIZ_ARRAY_LEN(known_size_array)\
  (sizeof((known_size_array)) / sizeof((known_size_array)[0]))
