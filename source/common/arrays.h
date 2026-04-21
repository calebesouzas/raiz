#pragma once

/**
 * @doc!
 * @name common/arrays.h
 * @type header
 * @tags helper
 * @desc contains helpers for common arrays operations and implementations.
 */

/**
 * @doc!
 * @name da_pattern
 * @type convention
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
 * @name da_append
 * @type macro
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
