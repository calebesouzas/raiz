#ifndef RAIZ_RUNTIME_SOURCE
#define RAIZ_RUNTIME_SOURCE

#include "runtime.h"
#include "parser.h"

static inline Rz_VM rz_vm_new(Rz_ExprArena *arena)
{
  return (Rz_VM) {.arena = arena };
}

int rz_eval(Rz_VM *vm, size_t current)
{
#define current() vm->arena->items[current]
  switch (current().kind)
  {
    case RZ_EXPR_LITERAL: return current().as.literal;
    case RZ_EXPR_UNARY:
      if (current().as.unary.op != RZ_OP_SUBTRACT)
        RZ_PANIC("use unary with '-' operator\n");

      current = current().as.unary.target_id;
      return -rz_eval(vm, current);
    case RZ_EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      rz_scope_insert(vm->scope, strndup(current().as.variable.symbol, current().as.variable.size), 
      int left = rz_eval(vm, current().as.binary.left_id);
      int right = rz_eval(vm, current().as.binary.right_id);

      switch (current().as.binary.op)
      {
        case RZ_OP_SUM:        return left + right;
        case RZ_OP_SUBTRACT:   return left - right;
        case RZ_OP_MULTIPLY:   return left * right;
        case RZ_OP_DIVIDE:     return left / right;
        case RZ_OP_MODULE:     return left % right;
        case RZ_OP_GREATER:    return left > right;
        case RZ_OP_LESS:       return left < right;
        case RZ_OP_BIT_OR:     return left | right;
        case RZ_OP_BIT_XOR:    return left ^ right;
        case RZ_OP_BIT_AND:    return left & right;
        case RZ_OP_BIT_RSHIFT: return left >> right;
        case RZ_OP_BIT_LSHIFT: return left << right;
        case RZ_OP_EQUAL:      return left == right;
        case RZ_OP_NOT_EQUAL:  return left != right;
        case RZ_OP_GREATER_EQ: return left >= right;
        case RZ_OP_LESS_EQ:    return left <= right;
        case RZ_OP_BOOL_AND:   return left && right;
        case RZ_OP_BOOL_OR:    return left || right;
        default: RZ_UNREACHABLE("operator\n");
      }
    case RZ_EXPR_VARIABLE:
    {
      int *value = rz_scope_get(&vm->scope,
          strndup(current().as.variable.symbol, current().as.variable.size));
      if (!value) RZ_PANIC("undefined variable: '%.*s\n'",
          current().as.variable.symbol, current().as.variable.size);
      return *value;
    }
    case RZ_EXPR_VOID: return 0;
    default: // switch (arena->items[current].kind)
    RZ_UNREACHABLE("expression kind\n");
  }
#undef current
}

int rz_eval_arena(Rz_VM *vm)
{
  return rz_eval(vm, vm->arena->count - 1);
}

#endif // RAIZ_RUNTIME_SOURCE
