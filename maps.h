#ifndef RAIZ_MAPS_H
#define RAIZ_MAPS_H

#include "common.h"
#include "values.h"

// Has to be a prime number, as longer as possible, to prevent collisions.
#define RAIZ_MAP_SIZE 97

// I really didn't know about this syntax
typedef struct Rz_StringDoublePair {
  Rz_String key;
  double value;
  struct Rz_StringDoublePair *next;
} Rz_StringDoublePair;

typedef struct {
  Rz_StringDoublePair *buckets[RAIZ_MAP_SIZE];
} Rz_StringDoubleMap;

#endif // RAIZ_MAPS_H
