#ifndef RAIZ_VALUES_SOURCE
#define RAIZ_VALUES_SOURCE

#include "values.h"
#include "common.h"

static inline Rz_Value rz_value_with_int(int value)
{
  return (Rz_Value) {.kind = RZ_VALUE_INT, .as.integer = value };
}

static inline Rz_Value rz_value_with_double(double value)
{
  return (Rz_Value) {.kind = RZ_VALUE_FLOAT, .as.floating = value };
}

static inline Rz_Value rz_value_with_bool(bool value)
{
  return (Rz_Value) {.kind = RZ_VALUE_BOOL, .as.boolean = value };
}

// sorry for the ugly code in this function...
Rz_Value rz_value_resolve_binary(Rz_Value left, Rz_Value right, Rz_Operator op)
{
  Rz_Value result;
  switch (op)
  {
    case RZ_OP_EQUAL:
    case RZ_OP_NOT_EQUAL:
      switch (left.kind)
      {
        case RZ_VALUE_INT:
          result = rz_value_with(left.as.integer  == right.as.integer);
        case RZ_VALUE_FLOAT:
          result = rz_value_with(left.as.floating == right.as.floating);
        case RZ_VALUE_BOOL:
          result = rz_value_with(left.as.boolean  == right.as.boolean);
      }
      return (op == RZ_OP_NOT_EQUAL)?
        rz_value_with_bool(!result.as.boolean) : result;
    case RZ_OP_BOOL_OR:
    case RZ_OP_BOOL_AND:
      if (left.kind != RZ_VALUE_BOOL || right.kind != RZ_VALUE_BOOL)
        RZ_PANIC("both values should be '@bool'\n");

      return (op == RZ_OP_BOOL_OR)? 
        rz_value_with_bool(left.as.boolean || right.as.boolean) :
        rz_value_with_bool(left.as.boolean && right.as.boolean);
    case RZ_OP_GREATER:
    case RZ_OP_LESS:
    case RZ_OP_GREATER_EQ:
    case RZ_OP_LESS_EQ:
      if (left.kind == RZ_VALUE_BOOL || right.kind == RZ_VALUE_BOOL)
        RZ_PANIC("cannot use '@bool' values in comparisons\n");

    case RZ_OP_BIT_AND:
    case RZ_OP_BIT_OR:
    case RZ_OP_BIT_XOR:
    case RZ_OP_BIT_RSHIFT:
    case RZ_OP_BIT_LSHIFT:
    case RZ_OP_SUM:
    case RZ_OP_SUBTRACT:
    case RZ_OP_MULTIPLY:
    case RZ_OP_DIVIDE:
    case RZ_OP_MODULE:
    case RZ_OP_BANG: RZ_PANIC("cannot BANG yet\n");
  }
}

#define rz_value_with(v) _Generic(v,\
int: rz_value_with_int(v),\
double: rz_value_with_double(v),\
bool: rz_value_with_bool(v))

#endif // RAIZ_VALUES_SOURCE
