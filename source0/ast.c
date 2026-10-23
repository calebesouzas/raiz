#include "libc.h"
#include "ast.h"

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

  fprintf(stream, "[%zu] ", expr->id);

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
