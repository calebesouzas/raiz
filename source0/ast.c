#include "libc.h"
#include "ast.h"

#define EXPR_ARENA_CAPACITY (4 * 1024 * 1024)
ExprArena *new_expr_arena(void)
{
  ExprArena *arena = malloc(sizeof(*arena));
  if (arena == NULL)
    return NULL;

  arena->nodes = malloc(EXPR_ARENA_CAPACITY);
  if (arena->nodes == NULL)
  {
    free(arena);
    return NULL;
  }

  arena->count = 0;
  arena->next = NULL;

  return arena;
}

void free_expr_arena(ExprArena *arena)
{
  ExprArena *current = arena;
  while (current != NULL)
  {
    if (current->nodes != NULL)
    {
      free(current->nodes);
    }

    ExprArena *previous = current;
    current = current->next;
    free(previous);
  }
}

Expr *new_expr_node(ExprArena *arena)
{
  ExprArena *current = arena;

  if ((current->count + 1) * sizeof(Expr) > EXPR_ARENA_CAPACITY)
  {
    current->next = new_expr_arena();
    if (current->next == NULL)
      return NULL;

    current = current->next;
  }

  return &current->nodes[current->count++];
}

#define INDENT 2
void dump_expr(Expr *expr, FILE *stream, uint32_t level)
{
  for (uint32_t i = 0; i < level * INDENT; i++)
  {
    putc(' ', stream);
  }

  if (expr == NULL)
  {
    fprintf(stream, "(null node)\n");
    return;
  }

  switch (expr->type)
  {
    case EXPR_LITERAL:
      fprintf(stream, "literal (%d)\n", expr->as.literal.value);
      break;
    case EXPR_GROUP:
      fprintf(stream, "group:\n");
      dump_expr(expr->as.group.inner, stream, level + 1);
      break;
    case EXPR_UNARY:
      fprintf(stream, "unary (%.*s):\n", __TOKEN_FMT(expr->as.unary.operator));
      dump_expr(expr->as.unary.inner, stream, level + 1);
      break;
    case EXPR_BINARY:
      fprintf(stream, "binary (%.*s):\n", __TOKEN_FMT(expr->as.binary.operator));
      dump_expr(expr->as.binary.left, stream, level + 1);
      dump_expr(expr->as.binary.right, stream, level + 1);
      break;
    case EXPR_DECLARATION:
      fprintf(stream, "declaration (%.*s):\n", __TOKEN_FMT(expr->as.declaration.identifier));
      dump_expr(expr->as.declaration.value, stream, level + 1);
      break;
    case EXPR_IDENTIFIER:
      fprintf(stream, "identifier '%.*s'\n", __TOKEN_FMT(expr->as.identifier));
      break;
    case EXPR_BLOCK:
      fprintf(stream, "block (count %zu):\n", expr->as.block.count);
      array_for(Expr*, node, &expr->as.block)
      {
        dump_expr(*node, stream, level + 1);
      }
      break;
  }
}

void dump_ast(Ast *ast, FILE *stream)
{
  if (ast == NULL)
    return;

  dump_expr(ast->root, stream, 0);
}
