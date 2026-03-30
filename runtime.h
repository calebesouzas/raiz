#ifndef RAIZ_RUNTIME_H
#define RAIZ_RUNTIME_H

#include "maps.h" // Rz_ValueMap
#include "parser.h" // Rz_ExprArena

typedef enum {
  RZ_SUCCESS,
  // RZ_COMPILE_ERROR,
  RZ_RUNTME_ERROR,
} Rz_Result;

typedef struct {
  Rz_Result result;
  Rz_ExprArena *arena;
  Rz_StringIntMap *scope; // only global at the moment.
} Rz_VM; // calm down, it is not done yet

#endif // RAIZ_RUNTIME_H
