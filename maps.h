#ifndef RAIZ_MAPS_H
#define RAIZ_MAPS_H

#include "common.h"
#include "values.h"

// Has to be a prime number, as longer as possible, to prevent collisions.
#define RAIZ_MAP_SIZE 97

// I really didn't know about this syntax
typedef struct Rz_Pair_String2Value {
  Rz_String key;
  Rz_Value value;
  struct Rz_Pair_String2Value *next;
} Rz_Pair_String2Value;

typedef struct {
  Rz_Pair_String2Value *buckets[RAIZ_MAP_SIZE];
} Rz_Map_String2Value;

#endif // RAIZ_MAPS_H
