#pragma once

/**
 * @doc!
 * @name(header) common/arrays.h
 * @tags helper
 * @desc contains helpers for common arrays operations and implementations.
 */

/**
 * @doc!
 * @name(convention) da
 * @desc shows how to use `raiz_da_*` helper macros.
 * Schema:
 * `da`: structure containing at least these three fields:
 * `T *items;` where `T` can be any type
 * `size_t count;` how many `T` elements were pushed into `items`
 * `size_t capacity;` how many `T` elements can be pushed into `items` in total
 * Shouldn't do:
 * increment/decrement `items` pointer directly
 * - doing so may cause segfaults or incorrect pointer reallocations
 */

#define RAIZ_DA_INITIAL_CAPACITY 256

/**
 * @doc!
 * @name(macro) da_append
 * @tags helper
 * @param `da` pointer to dynamic array (any structure following [@da_pattern])
 * @param `value` value to be pushed
 * @desc pushes `value` into `da`, increments `da->count`.
 * Deals with necessary (re)allocations.
 */
#define raiz_da_append(da, value)\
  do {\
    if ((da)->capacity == 0) {\
      (da)->capacity = RAIZ_DA_INITIAL_CAPACITY;\
      (da)->items = malloc(sizeof(*(da)->items)*(da)->capacity);\
    }\
    if ((da)->count >= (da)->capacity) {\
      (da)->capacity *= 2;\
      (da)->items = realloc((da)->items, sizeof(*(da)->items)*(da)->capacity);\
    }\
    if ((da)->items != NULL) {\
      (da)->items[(da)->count++] = (value);\
    }\
  } while (0)

#define raiz_da_free_malloced_items(da)\
  do {\
    for (size_t i = 0; i < (da)->count; i++) {\
      free((da)->items[i]);\
    }\
  } while (0)

#define RAIZ_ARRAY_LEN(known_size_array)\
  (sizeof((known_size_array)) / sizeof((known_size_array)[0]))
