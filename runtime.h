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
  Rz_StringDoubleMap *scope; // only global at the moment.
} Rz_VM; // calm down, it is not done yet

Rz_VM rz_vm_new(Rz_ExprArena *arena, Rz_StringDoubleMap *scope);
double rz_eval_arena(Rz_VM *vm);

#endif // RAIZ_RUNTIME_H
