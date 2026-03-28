#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

#include "raiz.h"

// It is good to have a type for some kinds of stuff.
// If we're going to have code which expects a specific kind.
typedef struct
{
  Operator op;
  size_t left_id, right_id;
} Expr_Binary;

typedef struct
{
  Operator op;
  size_t target_id;
} Expr_Unary;

typedef struct
{
  const char *data;
  size_t size;
} Expr_Variable;

typedef enum
{
  EXPR_LITERAL,
  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_VARIABLE,
} ExprKind;

typedef struct
{
  ExprKind kind;
  size_t id;
  bool in_arena;
  union
  {
    int literal;
    Expr_Binary binary;
    Expr_Unary unary;
    Expr_Variable variable;
  } as;
} Expr;

// A dynamic array, but it's not intended to be iterated through directly
typedef struct
{
  size_t count, capacity;
  Expr *items; // all the 'id' fields in 'Expr' and variants refer to this
} ExprArena;

typedef struct
{
  Lexer *lexer;
  Token current;
  Token next;
  ExprArena *arena;
  // Do we really need it? I think it's better than 'PANIC()'ing everywhere...
  char error_buffer[RAIZ_COLUMN_LIMIT + RAIZ_COLUMN_LIMIT / 2];
} Parser;

#endif // RAIZ_PARSER_H
