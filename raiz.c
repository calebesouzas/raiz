#include "common.h"
#include "parser.c"

Expr *parser_build_ast(ExprArena *arena, const char *source)
{
  Lexer lexer = lexer_new(source);
  Parser parser = parser_new(&lexer, arena);
  return parser_parse_expr(&parser, 0);
}

int eval(ExprArena *arena, size_t current)
{
  switch (arena->items[current].kind)
  {
    case EXPR_LITERAL: return arena->items[current].as.literal;
    case EXPR_UNARY:
      if (arena->items[current].as.unary.op != OP_SUBTRACT)
        PANIC("use unary with '-' operator\n");

      current = arena->items[current].as.unary.target_id;
      return -eval(arena, current);
    case EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      int left = eval(arena, arena->items[current].as.binary.left_id);
      int right = eval(arena, arena->items[current].as.binary.right_id);

      switch (arena->items[current].as.binary.op)
      {
        case OP_SUM:        return left + right;
        case OP_SUBTRACT:   return left - right;
        case OP_MULTIPLY:   return left * right;
        case OP_DIVIDE:     return left / right;
        case OP_MODULE:     return left % right;
        case OP_GREATER:    return left > right;
        case OP_LESS:       return left < right;
        case OP_BIT_OR:     return left | right;
        case OP_BIT_XOR:    return left ^ right;
        case OP_BIT_AND:    return left & right;
        case OP_BIT_RSHIFT: return left >> right;
        case OP_BIT_LSHIFT: return left << right;
        case OP_EQUAL:      return left == right;
        case OP_NOT_EQUAL:  return left != right;
        case OP_GREATER_EQ: return left >= right;
        case OP_LESS_EQ:    return left <= right;
        case OP_BOOL_AND:   return left && right;
        case OP_BOOL_OR:    return left || right;
        default: UNREACHABLE("eval(): operator\n");
      }
    default: // switch (arena->items[current].kind)
      UNREACHABLE("eval(): expression kind\n");
  }
}

void dump_ast(ExprArena *arena, Expr *ast, size_t level)
{
  for (size_t i = 0; i < level; i++) printf("  ");

  char *operators[] = {
    #define X(variant, token, bind_power) token,
    OP_X_MACRO_TABLE
    #undef X
  };

  switch (arena->items[ast->id].kind)
  {
    case EXPR_LITERAL:
      printf("Literal %d\n", ast->as.literal);
      break;
    case EXPR_UNARY:
      printf("Unary %s\n", operators[ast->as.unary.op]);
      dump_ast(arena, &arena->items[ast->as.unary.target_id], level + 1);
      break;
    case EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      printf("Binary %s\n", operators[ast->as.binary.op]);
      dump_ast(arena, &arena->items[ast->as.binary.left_id],  level + 1);
      dump_ast(arena, &arena->items[ast->as.binary.right_id], level + 1);
      break;

    default: // switch (arena->items[current].kind)
      UNREACHABLE("dump_ast(): expression kind\n");
  }
}

int eval_arena(ExprArena *arena)
{
  return eval(arena, arena->count - 1);
}

int main(void)
{
  char buffer[1024] = {0};

  printf("$> ");
  while (fgets(buffer, sizeof(buffer), stdin))
  {
    if (strncmp(buffer, "exit", 4) == 0) break;

    ExprArena arena = {0};
    Expr *ast = parser_build_ast(&arena, buffer);

    printf("---- AST ----\n");
    dump_ast(&arena, ast, 1);

    printf("Result %d\n", eval_arena(&arena));

    free(arena.items);
    printf("$> ");
  }
  return 0;
}
