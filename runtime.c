#ifndef RAIZ_RUNTIME_SOURCE
#define RAIZ_RUNTIME_SOURCE

#include "common.h"
#include "ast.h" // Rz_Operator
#include "runtime.h"
#include "parser.h"

Rz_VM rz_vm_new(Rz_ExprArena *arena, Rz_StringValueMap *scope)
{
  return (Rz_VM) {.arena = arena, .scope = scope };
}

/* many helpers because i'm very lazy */
static inline Rz_Expr get(Rz_VM *vm, size_t index);
static inline Rz_Expr current(Rz_VM *vm);
static inline Rz_VM *restore(Rz_VM *vm, size_t saved_current);
static inline Rz_Expr pop(Rz_VM *vm);
static inline bool match(Rz_VM *vm, Rz_ExprKind expected);
static inline Rz_Expr_Unary unary(Rz_VM *vm);
static inline Rz_Expr_Binary binary(Rz_VM *vm);
static inline Rz_String variable(Rz_VM *vm);
static inline Rz_Value literal(Rz_VM *vm);
static inline bool values_compatible(Rz_Value left, Rz_Value right);
static inline const char *extract_operator_kind(Rz_Operator op)
{
  switch (op)
  {
    case RZ_OP_ASSIGN: return "assignment";
    case RZ_OP_EQUAL:
    case RZ_OP_NOT_EQUAL:
    case RZ_OP_GREATER:
    case RZ_OP_LESS:
    case RZ_OP_GREATER_EQ:
    case RZ_OP_LESS_EQ:
      return "comparison";
    case RZ_OP_BOOL_OR:
      return "boolean or";
    case RZ_OP_BOOL_AND:
      return "boolean and";
    case RZ_OP_BIT_AND:
    case RZ_OP_BIT_OR:
    case RZ_OP_BIT_XOR:
    case RZ_OP_BIT_RSHIFT:
    case RZ_OP_BIT_LSHIFT:
      return "bitwise";
    case RZ_OP_SUM:
      return "sum";
    case RZ_OP_SUBTRACT:
      return "subtract";
    case RZ_OP_MULTIPLY:
      return "multiplication";
    case RZ_OP_DIVIDE:
      return "division";
    case RZ_OP_MODULE:
      return "module";
    case RZ_OP_BANG:
      return "explosion";
  }
}

// bro, look at the work that tagged unions cause!
static Rz_Value negative(Rz_Value value);
static Rz_Value sum(Rz_Value left, Rz_Value right);
static Rz_Value subtract(Rz_Value left, Rz_Value right);
static Rz_Value multiply(Rz_Value left, Rz_Value right);
static Rz_Value divide(Rz_Value left, Rz_Value right);
static Rz_Value module(Rz_Value left, Rz_Value right);
static Rz_Value greater(Rz_Value left, Rz_Value right);
static Rz_Value less(Rz_Value left, Rz_Value right);
static Rz_Value bit_or(Rz_Value left, Rz_Value right);
static Rz_Value bit_xor(Rz_Value left, Rz_Value right);
static Rz_Value bit_and(Rz_Value left, Rz_Value right);
static Rz_Value bit_rshift(Rz_Value left, Rz_Value right);
static Rz_Value bit_lshift(Rz_Value left, Rz_Value right);
static Rz_Value equal(Rz_Value left, Rz_Value right);
static Rz_Value not_equal(Rz_Value left, Rz_Value right);
static Rz_Value greater_eq(Rz_Value left, Rz_Value right);
static Rz_Value less_eq(Rz_Value left, Rz_Value right);
static Rz_Value bool_and(Rz_Value left, Rz_Value right);
static Rz_Value bool_or(Rz_Value left, Rz_Value right);

Rz_Value rz_eval(Rz_VM *vm, size_t index)
{
  size_t saved = vm->__current;
  vm->__current = index;

  switch (current(vm).kind)
  {
    case RZ_EXPR_LITERAL: return literal(vm);
    case RZ_EXPR_UNARY:
      if (unary(vm).op != RZ_OP_SUBTRACT)
        RZ_PANIC("use unary with '-' operator\n");

      return negative(rz_eval(vm, unary(vm).target_id));
    case RZ_EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      if (binary(vm).op == RZ_OP_ASSIGN)
      {
        Rz_Value value = rz_eval(vm, binary(vm).right_id);

        if (!match(vm, RZ_EXPR_VARIABLE))
        {
          RZ_PANIC("cannot assign value to non-variable left side\n");
        }
        rz_scope_insert(vm->scope, variable(vm), value);

        return value;
      }

      Rz_Value left = rz_eval(vm, binary(vm).left_id);

      Rz_Value right = rz_eval(vm, binary(restore(vm, saved)).right_id);
      (void) restore(vm, saved);

      if (!values_compatible(left, right))
        RZ_PANIC("cannot use %s operation with %s and %s\nExpressions should be of the same type\n",
            extract_operator_kind(binary(vm).op),
            rz_value_extract_kind(left),
            rz_value_extract_kind(right));

      // TODO: implement binary operations with `Rz_Value`s
      switch (binary(vm).op)
      {
        case RZ_OP_SUM:        return sum(left, right);
        case RZ_OP_SUBTRACT:   return subtract(left, right);
        case RZ_OP_MULTIPLY:   return multiply(left, right);
        case RZ_OP_DIVIDE:     return divide(left, right);
        case RZ_OP_MODULE:     return module(left, right);
        case RZ_OP_GREATER:    return greater(left, right);
        case RZ_OP_LESS:       return less(left, right);
        case RZ_OP_BIT_OR:     return bit_or(left, right);
        case RZ_OP_BIT_XOR:    return bit_xor(left, right);
        case RZ_OP_BIT_AND:    return bit_and(left, right);
        case RZ_OP_BIT_RSHIFT: return bit_rshift(left, right);
        case RZ_OP_BIT_LSHIFT: return bit_lshift(left, right);
        case RZ_OP_EQUAL:      return equal(left, right);
        case RZ_OP_NOT_EQUAL:  return not_equal(left, right);
        case RZ_OP_GREATER_EQ: return greater_eq(left, right);
        case RZ_OP_LESS_EQ:    return less_eq(left, right);
        case RZ_OP_BOOL_AND:   return bool_and(left, right);
        case RZ_OP_BOOL_OR:    return bool_or(left, right);
        default: RZ_UNREACHABLE("operator");
      }
    case RZ_EXPR_VARIABLE:
    {
      Rz_Value *value = rz_scope_get(vm->scope, variable(vm));
      if (!value) RZ_PANIC("undefined variable: '%.*s'\n", RZ_SV(variable(vm)));
      return *value;
    }
    case RZ_EXPR_VOID: return rz_value_void();
    default: // switch (arena->items[current].kind)
    RZ_UNREACHABLE("expression kind");
  }
}

Rz_Value sum(Rz_Value left, Rz_Value right)
{
  switch (left.kind)
  {
    case RZ_VALUE_INT: return rz_value_int(left.as.integer + right.as.integer);
    case RZ_VALUE_FLOAT: return rz_value_float(left.as.floating + right.as.floating);
    case RZ_VALUE_STRING:
      Rz_String result = {0};
      result.data = malloc(left.as.string.size + right.as.string.size);
      // concat 'left' and 'right' into 'result'
      if (result.data != NULL)
      {
        strncpy(result.data, left.as.string.data, left.as.string.size);
        strncpy(result.data + left.as.string.size, right.as.string.data, right.as.string.size);
        result.size = left.as.string.size + right.as.string.size;
      }
      return rz_value_string(result);
    case RZ_VALUE_BOOL:
    case RZ_VALUE_VOID:
      RZ_PANIC("cannot sum '%s' values\n", rz_value_extract_kind(left));
    default: RZ_UNREACHABLE("value kind");
  }
}

static Rz_Value negative(Rz_Value value)
{
  switch (value.kind)
  {
    case RZ_VALUE_INT: return rz_value_int(-value.as.integer);
    case RZ_VALUE_FLOAT: return rz_value_float(-value.as.floating);
    case RZ_VALUE_STRING:
    case RZ_VALUE_BOOL:
    case RZ_VALUE_VOID:
      RZ_PANIC("type '%s' cannot negative\n", rz_value_extract_kind(value));
    default: RZ_UNREACHABLE("value kind");
  }
}

static Rz_Value subtract(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value subtract()");
}

static Rz_Value multiply(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value multiply()");
}

static Rz_Value divide(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value divide()");
}
static Rz_Value module(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value module()");
}

static Rz_Value greater(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value greater()");
}

static Rz_Value less(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value less()");
}

static Rz_Value bit_or(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value bit_or()");
}

static Rz_Value bit_xor(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value bit_xor()");
}

static Rz_Value bit_and(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value bit_and()");
}

static Rz_Value bit_rshift(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value bit_rshift()");
}

static Rz_Value bit_lshift(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value bit_lshift()");
}

static Rz_Value equal(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value equal()");
}

static Rz_Value not_equal(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value not_equal()");
}

static Rz_Value greater_eq(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value greater_eq()");
}

static Rz_Value less_eq(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value less_eq()");
}

static Rz_Value bool_and(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value bool_and()");
}

static Rz_Value bool_or(Rz_Value left, Rz_Value right)
{
  RZ_TODO("Rz_Value bool_or()");
}

Rz_Value rz_eval_arena(Rz_VM *vm) { vm->__current = vm->arena->count - 1; return rz_eval(vm, vm->__current); }

static inline Rz_Expr get(Rz_VM *vm, size_t index) { return vm->arena->items[index]; }

static inline Rz_Expr current(Rz_VM *vm) { return vm->arena->items[vm->__current]; }

static inline Rz_VM *restore(Rz_VM *vm, size_t saved_current) { vm->__current = saved_current; return vm; }

static inline Rz_Expr pop(Rz_VM *vm) { return vm->arena->items[vm->__current--]; }

static inline bool match(Rz_VM *vm, Rz_ExprKind expected) { return current(vm).kind == expected; }

static inline Rz_Expr_Unary unary(Rz_VM *vm) { return current(vm).as.unary; }

static inline Rz_Expr_Binary binary(Rz_VM *vm) { return current(vm).as.binary; }

static inline Rz_String variable(Rz_VM *vm) { return current(vm).as.variable; }

static inline Rz_Value literal(Rz_VM *vm) { return current(vm).as.literal; }

static inline bool values_compatible(Rz_Value left, Rz_Value right) { return left.kind == right.kind; }

#endif // RAIZ_RUNTIME_SOURCE
