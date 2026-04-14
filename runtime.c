#ifndef RAIZ_RUNTIME_SOURCE
#define RAIZ_RUNTIME_SOURCE

#include "common.h"
#include "ast.h" // Rz_Operator
#include "runtime.h"
#include "parser.h"

Rz_VM rz_vm_new(Rz_ExprArena *arena, Rz_StringDoubleMap *scope)
{
  return (Rz_VM) {.arena = arena, .scope = scope };
}

static inline Rz_Expr current(Rz_VM *vm)
{
  return vm->arena->items[vm->arena->current];
}

static inline Rz_Expr_Unary unary(Rz_VM *vm)
{
  return current(vm).as.unary;
}

static inline Rz_Expr_Binary binary(Rz_VM *vm)
{
  return current(vm).as.binary;
}

static inline Rz_String variable(Rz_VM *vm)
{
  return current(vm).as.variable;
}

static inline int literal(Rz_VM *vm)
{
  return current(vm).as.literal;
}

static inline void save(Rz_VM *vm, size_t new_current)
{
  vm->arena->current = new_current;
}

static inline bool match(Rz_VM *vm, Rz_ExprKind expected)
{
  return current(vm).kind == expected;
}

double rz_eval(Rz_VM *vm)
{
  switch (current(vm).kind)
  {
    case RZ_EXPR_LITERAL: return literal(vm);
    case RZ_EXPR_UNARY:
      if (unary(vm).op != RZ_OP_SUBTRACT)
        RZ_PANIC("use unary with '-' operator\n");

      save(vm, unary(vm).target_id);
      return -rz_eval(vm);
    case RZ_EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      size_t saved = vm->arena->current;
      if (binary(vm).op == RZ_OP_ASSIGN)
      {
        save(vm, binary(vm).right_id);
        double value = rz_eval(vm);

        save(vm, saved); // restore current binary
        save(vm, binary(vm).left_id); // variable
        if (!match(vm, RZ_EXPR_VARIABLE))
        {
          RZ_PANIC("cannot assign value to non-variable left side\n");
        }
        rz_scope_insert(vm->scope, variable(vm), value);
        save(vm, saved);
        return value;
      }

      save(vm, binary(vm).left_id);
      double left = rz_eval(vm);
      save(vm, saved);

      save(vm, binary(vm).right_id);
      double right = rz_eval(vm);
      save(vm, saved);

      switch (binary(vm).op)
      {
        case RZ_OP_SUM:        return left + right;
        case RZ_OP_SUBTRACT:   return left - right;
        case RZ_OP_MULTIPLY:   return left * right;
        case RZ_OP_DIVIDE:     return left / right;
        // case RZ_OP_MODULE:     return left % right;
        case RZ_OP_GREATER:    return left > right;
        case RZ_OP_LESS:       return left < right;
        // case RZ_OP_BIT_OR:     return left | right;
        // case RZ_OP_BIT_XOR:    return left ^ right;
        // case RZ_OP_BIT_AND:    return left & right;
        // case RZ_OP_BIT_RSHIFT: return left >> right;
        // case RZ_OP_BIT_LSHIFT: return left << right;
        case RZ_OP_EQUAL:      return left == right;
        case RZ_OP_NOT_EQUAL:  return left != right;
        case RZ_OP_GREATER_EQ: return left >= right;
        case RZ_OP_LESS_EQ:    return left <= right;
        case RZ_OP_BOOL_AND:   return left && right;
        case RZ_OP_BOOL_OR:    return left || right;
        default: RZ_UNREACHABLE("operator");
      }
    case RZ_EXPR_VARIABLE:
    {
      double *value = rz_scope_get(vm->scope, variable(vm));
      if (!value) RZ_PANIC("undefined variable: '%.*s'\n", RZ_SV(variable(vm)));
      return *value;
    }
    case RZ_EXPR_VOID: return 0;
    default: // switch (arena->items[current].kind)
    RZ_UNREACHABLE("expression kind");
  }
}

#undef current
#undef unary
#undef binary
#undef variable
#undef literal
#undef save

double rz_eval_arena(Rz_VM *vm)
{
  return rz_eval(vm);
}

#endif // RAIZ_RUNTIME_SOURCE
