#pragma once

#include <stdint.h>

#define RAIZ_BP_PAIR(left, right)\
  ((uint16_t) ((uint8_t) ((left) << 8) + (uint8_t) (right)))

#define RAIZ_BP_LEFT(pair) ((uint8_t) ((pair) >> 8))
#define RAIZ_BP_RIGHT(pair) ((uint8_t) ((pair) - 256))

typedef enum
{
  RAIZ_OP_ASSIGN   = RAIZ_BP_PAIR(10, 20),
  RAIZ_OP_LESS     = RAIZ_BP_PAIR(30, 40),
  RAIZ_OP_LESS_EQ,
  RAIZ_OP_GREATER,
  RAIZ_OP_GREATER_EQ,
  RAIZ_OP_EQUAL,
  RAIZ_OP_NOT_EQUAL,
  RAIZ_OP_SUM      = RAIZ_BP_PAIR(50, 60),
  RAIZ_OP_SUBTRACT,
  RAIZ_OP_MULTIPLY = RAIZ_BP_PAIR(70, 80),
  RAIZ_OP_DIVIDE,
  RAIZ_OP_BOOL_OR  = RAIZ_BP_PAIR(90, 100),
  RAIZ_OP_BOOL_AND,
  RAIZ_OP_BIT_OR   = RAIZ_BP_PAIR(110, 120),
  RAIZ_OP_BIT_AND,
  RAIZ_OP_BIT_XOR,
  RAIZ_OP_SHIFT_L,
  RAIZ_OP_SHIFT_R,
} Raiz_Op,

typedef enum
{
  RAIZ_EXPR_VOID, // literal value representing absence of something
  RAIZ_EXPR_LITERAL, // any primitive value
  RAIZ_EXPR_SYMBOL, // a single identifier
  RAIZ_EXPR_UNARY, // an operator and then another expression
  RAIZ_EXPR_BINARY, // two sides and an infix operator
  RAIZ_EXPR_GROUP, // when inside parentheses
  RAIZ_EXPR_CHECK, // sometimes is ternary, but taken to another level
  RAIZ_EXPR_PRINT, // side effect: resolves the arguments and prints the result
  RAIZ_EXPR_BLOCK, // when inside curly braces
} Raiz_ExprKind;

typedef struct
{
  Raiz_ExprKind kind;
  size_t id;
  union
  {
    struct
    {
      int v_int;
    } literal;
    Raiz_String symbol;
    struct
    {
      size_t target_id;
      Raiz_Op op;
    } unary;
    struct
    {
      size_t left_id, right_id;
      Raiz_Op op;
    } binary;
    struct
    {
      size_t inner_id;
    } group;
    struct
    {
      size_t condition_id;
      size_t block_id;
    } check;
    struct
    {
      size_t target_id;
    } print;
  };
} Raiz_Expr;

Raiz_Expr raiz_expr_literal(int value);
Raiz_Expr raiz_expr_symbol(Raiz_String symbol);
Raiz_Expr raiz_expr_unary(Raiz_Expr *target);
Raiz_Expr raiz_expr_binary(Raiz_Expr *left, Raiz_Expr *right, Raiz_Op op);
Raiz_Expr raiz_expr_group(Raiz_Expr *inner);
Raiz_Expr raiz_expr_check(Raiz_Expr *condition, Raiz_Expr *block);
Raiz_Expr raiz_expr_print(Raiz_Expr *target);
