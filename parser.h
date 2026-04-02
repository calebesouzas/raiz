#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

#include "common.h"
#include "values.h"

// It is good to have a type for some kinds of stuff.
// If we're going to have code which expects a specific kind.
typedef struct
{
  Rz_Operator op;
  size_t left_id, right_id;
} Rz_Expr_Binary;

typedef struct
{
  Rz_Operator op;
  size_t target_id;
} Rz_Expr_Unary;

typedef enum
{
  RZ_EXPR_LITERAL,
  RZ_EXPR_UNARY,
  RZ_EXPR_BINARY,
  RZ_EXPR_VARIABLE,
  RZ_EXPR_VOID,
} Rz_ExprKind;

typedef struct
{
  Rz_ExprKind kind;
  size_t id;
  bool in_arena;
  union
  {
    Rz_Value literal;
    Rz_Expr_Binary binary;
    Rz_Expr_Unary unary;
    Rz_String variable;
  } as;
} Rz_Expr;

// A dynamic array, but it's not intended to be iterated through directly
typedef struct
{
  size_t count, capacity;
  size_t current; // for consuming nodes
  Rz_Expr *items; // all the 'id' fields in 'Expr' and variants refer to this
} Rz_ExprArena;

typedef struct
{
  Rz_Lexer *lexer;
  Rz_Token current;
  Rz_Token next;
  Rz_ExprArena *arena;
  // Do we really need it? I think it's better than 'PANIC()'ing everywhere...
  char *error_buffer;
} Rz_Parser;

#endif // RAIZ_PARSER_H
