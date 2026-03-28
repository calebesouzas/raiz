#include "common.h"
#include "lexer.c"
#include "parser.c"

int rz_eval(Rz_ExprArena *arena, size_t current)
{
  switch (arena->items[current].kind)
  {
    case RZ_EXPR_LITERAL: return arena->items[current].as.literal;
    case RZ_EXPR_UNARY:
      if (arena->items[current].as.unary.op != RZ_OP_SUBTRACT)
        RZ_PANIC("use unary with '-' operator\n");

      current = arena->items[current].as.unary.target_id;
      return -rz_eval(arena, current);
    case RZ_EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      int left = rz_eval(arena, arena->items[current].as.binary.left_id);
      int right = rz_eval(arena, arena->items[current].as.binary.right_id);

      switch (arena->items[current].as.binary.op)
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
        default: RZ_UNREACHABLE("eval(): operator\n");
      }
    default: // switch (arena->items[current].kind)
    RZ_UNREACHABLE("eval(): expression kind\n");
  }
}

int rz_eval_arena(Rz_ExprArena *arena)
{
  return rz_eval(arena, arena->count - 1);
}

int main(void)
{
  char buffer[1024] = {0};

  printf("$> ");
  while (fgets(buffer, sizeof(buffer), stdin))
  {
    if (strncmp(buffer, "exit", 4) == 0) break;

    Rz_ExprArena arena = {0};
    (void)rz_parser_build_ast(&arena, buffer);

    printf("Result %d\n", rz_eval_arena(&arena));

    free(arena.items);
    printf("$> ");
  }
  return 0;
}
