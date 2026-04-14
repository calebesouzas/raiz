#ifndef RAIZ_MAPS_H
#define RAIZ_MAPS_H

#include "common.h"
#include "values.h"

// Has to be a prime number, as longer as possible, to prevent collisions.
#define RAIZ_MAP_SIZE 97

// I really didn't know about this syntax
typedef struct Rz_StringValuePair {
  Rz_String key;
  Rz_Value value;
  struct Rz_StringValuePair *next;
} Rz_StringValuePair;

typedef struct {
  Rz_StringValuePair *buckets[RAIZ_MAP_SIZE];
} Rz_StringValueMap;

void rz_scope_insert(Rz_StringValueMap *map, Rz_String key, Rz_Value value);
Rz_Value *rz_scope_get(Rz_StringValueMap *map, Rz_String key);

#endif // RAIZ_MAPS_H
