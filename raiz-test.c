#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

enum Operator {
  OP_SUM,
  OP_SUB,
  OP_MULT,
  OP_DIV,
};

struct Expr_Binary {
  enum Operator op;
  size_t left_id, right_id;
};

struct Expr_Unary {
  enum Operator op;
  size_t target_id;
};

enum ExprKind {
  EXPR_LITERAL,
  EXPR_UNARY,
  EXPR_BINARY,
};

typedef struct {
  enum ExprKind kind;
  size_t id;
  union {
    int literal;
    struct Expr_Binary binary;
    struct Expr_Unary unary;
  } as;
} Expr;


typedef struct {
  size_t count, capacity;
  Expr* items;
} ExprArena;


void push_expr(ExprArena* arena, Expr* expr) {
  if (arena->capacity == 0) {
    arena->capacity = 256;
    arena->items = malloc(sizeof(Expr) * arena->capacity);
  }
  if (arena->count >= arena->capacity) {
    arena->capacity *= 2;
    arena->items = realloc(arena->items, sizeof(Expr) * arena->capacity);
  }
  if (arena->items != NULL) {
    expr->id = arena->count;
    arena->items[arena->count++] = *expr;
  }
}


Expr* expr_binary(ExprArena* arena, Expr* left, Expr* right, enum Operator op) {
  Expr result = (Expr) {
    .kind = EXPR_BINARY,
    .as.binary = (struct Expr_Binary) {
      .left_id = left->id,
      .right_id = right->id,
      .op = op,
    }
  };
  push_expr(arena, &result);
  return &arena->items[result.id];
}

Expr* expr_unary(ExprArena* arena, Expr* target) {
  Expr result = (Expr) {
    .kind = EXPR_UNARY,
    .as.unary = (struct Expr_Unary) {
      .op = OP_SUB,
      .target_id = target->id,
    }
  };
  push_expr(arena, &result);
  return &arena->items[result.id];
}

Expr* expr_literal(ExprArena* arena, int value) {
  Expr result = (Expr) {
    .kind = EXPR_LITERAL,
    .as.literal = value,
  };
  push_expr(arena, &result);
  return &arena->items[result.id];
}


int eval(ExprArena* arena, size_t current) {
  switch (arena->items[current].kind) {
  case EXPR_LITERAL: return arena->items[current].as.literal;
  case EXPR_UNARY:
    if (arena->items[current].as.unary.op != OP_SUB) {
      fprintf(stderr, "use unary with '-' operator\n");
      abort();
    }
    current = arena->items[current].as.unary.target_id;
    return -eval(arena, current);
  case EXPR_BINARY:
    ; // this thing literally made "declaration after label is a C23 extension" warning disappear!
    int left = eval(arena, arena->items[current].as.binary.left_id);
    int right = eval(arena, arena->items[current].as.binary.right_id);

    switch (arena->items[current].as.binary.op) {
    case OP_SUM:  return left + right;
    case OP_SUB:  return left - right;
    case OP_MULT: return left * right;
    case OP_DIV:  return left / right;
    default:
      fprintf(stderr, "reached unreachable operator\n");
      abort();
    }
  default: // switch (arena->items[current].kind)
    fprintf(stderr, "reached unreachable expression kind\n");
    abort();
  }
}

int main(void) {
  ExprArena arena = {0};
 
  // 10 + 3 * 5
  // 10 + 15
  // 25
  Expr* ast = expr_binary(&arena, expr_literal(&arena, 10), expr_binary(&arena, expr_literal(&arena, 3), expr_literal(&arena, 5), OP_MULT), OP_SUM);
  printf("Result: %d\n", eval(&arena, ast->id));

  // -96
  arena.count = 0;
  Expr* unary = expr_unary(&arena, expr_literal(&arena, 96));
  printf("Result: %d\n", eval(&arena, unary->id));

  free(arena.items);
  return 0;
}
