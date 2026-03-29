#ifndef RAIZ_MAPS_H
#define RAIZ_MAPS_H

#include "values.h"

// Has to be a prime number, as longer as possible, to prevent collisions.
#define RAIZ_MAP_SIZE 97

// I really didn't know about this syntax
typedef struct Rz_StringIntPair {
  char *key;
  int *value;
  struct Rz_StringIntPair *next;
} Rz_StringIntPair;

typedef struct {
  Rz_StringIntPair *buckets[RAIZ_MAP_SIZE];
} Rz_StringIntMap;

#endif // RAIZ_MAPS_H
