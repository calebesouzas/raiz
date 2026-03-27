#ifndef RAIZ_EXPRESSIONS_SOURCE
#define RAIZ_EXPRESSIONS_SOURCE

uint16_t get_binding_power(Operator op)
{
  uint16_t powers[] = {
    #define X(variant, binding_power) binding_power,
    OP_X_MACRO_TABLE
    #undef X
  };

  return powers[op];
}

void push_expr(ExprArena *arena, Expr *expr)
{
  if (arena->capacity == 0)
  {
    arena->capacity = 256;
    arena->items = malloc(sizeof(Expr) * arena->capacity);
  }
  if (arena->count >= arena->capacity)
  {
    arena->capacity *= 2;
    arena->items = realloc(arena->items, sizeof(Expr) * arena->capacity);
  }
  if (!expr->in_arena && arena->items != NULL)
  {
    expr->id = arena->count;
    arena->items[arena->count++] = *expr;
    expr->in_arena = true;
  }
}

Expr *expr_binary(ExprArena *arena, Expr *left, Expr *right, Operator op)
{
  Expr result = (Expr) {
    .kind = EXPR_BINARY,
    .as.binary = (Expr_Binary) {
      .left_id = left->id,
      .right_id = right->id,
      .op = op,
    }
  };
  push_expr(arena, &result);
  assert(result.in_arena);
  return &arena->items[result.id];
}

Expr *expr_unary(ExprArena *arena, Expr *target)
{
  Expr result = (Expr) {
    .kind = EXPR_UNARY,
    .as.unary = (Expr_Unary) {
      .op = OP_SUBTRACT,
      .target_id = target->id,
    }
  };
  push_expr(arena, &result);
  assert(result.in_arena);
  return &arena->items[result.id];
}

Expr *expr_literal(ExprArena *arena, int value)
{
  Expr result = (Expr) {
    .kind = EXPR_LITERAL,
    .as.literal = value,
  };
  push_expr(arena, &result);
  assert(result.in_arena);
  return &arena->items[result.id];
}

#endif // RAIZ_EXPRESSIONS_SOURCE
