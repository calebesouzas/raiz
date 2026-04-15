#include "common.h"
#include "ast.h"

uint16_t rz_get_binding_power(Rz_Operator op)
{
  uint16_t powers[] = {
    #define X(variant, binding_power) binding_power,
    RZ_OP_X_MACRO_TABLE
    #undef X
  };

  return powers[op];
}

void rz_push_expr(Rz_ExprArena *arena, Rz_Expr *expr)
{
  if (arena->capacity == 0)
  {
    arena->capacity = 256;
    arena->items = malloc(sizeof(Rz_Expr) * arena->capacity);
  }
  if (arena->count >= arena->capacity)
  {
    arena->capacity *= 2;
    arena->items = realloc(arena->items, sizeof(Rz_Expr) * arena->capacity);
  }
  if (!expr->in_arena && arena->items != NULL)
  {
    expr->id = arena->count;
    arena->items[arena->count++] = *expr;
    expr->in_arena = true;
  }
}

Rz_Expr *rz_expr_void(Rz_ExprArena *arena)
{
  Rz_Expr result = {.kind = RZ_EXPR_VOID };
  rz_push_expr(arena, &result);
  return &arena->items[result.id];
}

Rz_Expr *rz_expr_binary(Rz_ExprArena *arena, Rz_Expr *left, Rz_Expr *right, Rz_Operator op)
{
  Rz_Expr result = (Rz_Expr) {
    .kind = RZ_EXPR_BINARY,
    .as.binary = (Rz_Expr_Binary) {
      .left_id = left->id,
      .right_id = right->id,
      .op = op,
    }
  };
  rz_push_expr(arena, &result);
  return &arena->items[result.id];
}

Rz_Expr *rz_expr_unary(Rz_ExprArena *arena, Rz_Expr *target)
{
  Rz_Expr result = (Rz_Expr) {
    .kind = RZ_EXPR_UNARY,
    .as.unary = (Rz_Expr_Unary) {
      .op = RZ_OP_SUBTRACT,
      .target_id = target->id,
    }
  };
  rz_push_expr(arena, &result);
  return &arena->items[result.id];
}

Rz_Expr *rz_expr_literal(Rz_ExprArena *arena, int value)
{
  Rz_Expr result = (Rz_Expr) {
    .kind = RZ_EXPR_LITERAL,
    .as.literal = value,
  };
  rz_push_expr(arena, &result);
  return &arena->items[result.id];
}

Rz_Expr *rz_expr_variable(Rz_ExprArena *arena, const char *symbol, size_t size)
{
  Rz_Expr result = (Rz_Expr) {
    .kind = RZ_EXPR_VARIABLE,
    .as.variable = rz_string_cstr_slice(symbol, size)
  };
  rz_push_expr(arena, &result);
  return &arena->items[result.id];
}
