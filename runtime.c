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

Rz_Value rz_eval(Rz_VM *vm, size_t index)
{
#define return_defer(value)\
  do\
  {\
    vm->arena->count = saved;\
    return value;\
  } while (0)

  size_t saved = vm->arena->count;
  vm->arena->count = index;

  switch (current(vm).kind)
  {
    case RZ_EXPR_LITERAL: return_defer(literal(vm));
    case RZ_EXPR_UNARY:
      if (unary(vm).op != RZ_OP_SUBTRACT)
        RZ_PANIC("use unary with '-' operator\n");

      return_defer(-rz_eval(vm, unary(vm).target_id));
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

        return_defer(value);
      }

      Rz_Value left = rz_eval(vm, current(vm).left_id);

      Rz_Value right = rz_eval(vm, current(vm).right_id);

      if (!values_compatible(left, right))
        RZ_PANIC("cannot use %s operation with %s and %s\nExpressions should be of the same type\n",
            extract_operator_kind(binary(vm).op),
            rz_value_extract_kind(left),
            rz_value_extract_kind(right));

      // TODO: implement binary operations with `Rz_Value`s
      switch (binary(vm).op)
      {
        case RZ_OP_SUM:        return_defer(left + right);
        case RZ_OP_SUBTRACT:   return_defer(left - right);
        case RZ_OP_MULTIPLY:   return_defer(left * right);
        case RZ_OP_DIVIDE:     return_defer(left / right);
        case RZ_OP_MODULE:     return_defer(left % right);
        case RZ_OP_GREATER:    return_defer(left > right);
        case RZ_OP_LESS:       return_defer(left < right);
        case RZ_OP_BIT_OR:     return_defer(left | right);
        case RZ_OP_BIT_XOR:    return_defer(left ^ right);
        case RZ_OP_BIT_AND:    return_defer(left & right);
        case RZ_OP_BIT_RSHIFT: return_defer(left >> right);
        case RZ_OP_BIT_LSHIFT: return_defer(left << right);
        case RZ_OP_EQUAL:      return_defer(left == right);
        case RZ_OP_NOT_EQUAL:  return_defer(left != right);
        case RZ_OP_GREATER_EQ: return_defer(left >= right);
        case RZ_OP_LESS_EQ:    return_defer(left <= right);
        case RZ_OP_BOOL_AND:   return_defer(left && right);
        case RZ_OP_BOOL_OR:    return_defer(left || right);
        default: RZ_UNREACHABLE("operator");
      }
    case RZ_EXPR_VARIABLE:
    {
      Rz_Value *value = rz_scope_get(vm->scope, variable(vm));
      if (!value) RZ_PANIC("undefined variable: '%.*s'\n", RZ_SV(variable(vm)));
      return_defer(*value);
    }
    case RZ_EXPR_VOID: return_defer(rz_value_void());
    default: // switch (arena->items[current].kind)
    RZ_UNREACHABLE("expression kind");
  }
}

Rz_Value rz_eval_arena(Rz_VM *vm)
{
  return rz_eval(vm, vm->arena->count - 1);
}

static inline Rz_Expr get(Rz_VM *vm, size_t index) { return vm->arena->items[index]; }

static inline Rz_Expr current(Rz_VM *vm) { return vm->arena->items[vm->arena->count - 1]; }

static inline Rz_Expr pop(Rz_VM *vm) { return vm->arena->items[vm->arena->count--]; }

static inline bool match(Rz_VM *vm, Rz_ExprKind expected) { return current(vm).kind == expected; }

static inline Rz_Expr_Unary unary(Rz_VM *vm) { return current(vm).as.unary; }

static inline Rz_Expr_Binary binary(Rz_VM *vm) { return current(vm).as.binary; }

static inline Rz_String variable(Rz_VM *vm) { return current(vm).as.variable; }

static inline Rz_Value literal(Rz_VM *vm) { return current(vm).as.literal; }

static inline bool values_compatible(Rz_Value left, Rz_Value right) { return left.kind == right.kind; }

#endif // RAIZ_RUNTIME_SOURCE
