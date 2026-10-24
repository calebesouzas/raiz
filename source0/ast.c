#include "libc.h"
#include "ast.h"

typedef void (*WalkerFunction) (Expr *node, unsigned int depth, void *data);

void expr_walk(Expr *node, unsigned int depth, WalkerFunction function, void *data)
{
  function(node, depth, data);

  if (node == NULL)
    return;

  switch (node->type)
  {
    case EXPR_LITERAL:
    case EXPR_IDENTIFIER:
      break;
    case EXPR_GROUP:
      expr_walk(node->as.group.inner, depth + 1, function, data);
      break;
    case EXPR_UNARY:
      expr_walk(node->as.unary.inner, depth + 1, function, data);
      break;
    case EXPR_BINARY:
      expr_walk(node->as.binary.left, depth + 1, function, data);
      expr_walk(node->as.binary.right, depth + 1, function, data);
      break;
    case EXPR_DECLARATION:
      expr_walk(node->as.declaration.value, depth + 1, function, data);
      break;
    case EXPR_BLOCK:
      array_for(Expr*, term, &node->as.block)
      {
        expr_walk(*term, depth + 1, function, data);
      }
      break;
  }
}

void ast_walk(Ast *ast, WalkerFunction function, void *data)
{
  expr_walk(ast->root, 0, function, data);
}

#define INDENT 2
void dump_expr(Expr *expr, uint32_t level, void *data)
{
  FILE *stream = (FILE*) data;
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
      break;
    case EXPR_UNARY:
      fprintf(stream, "unary (%.*s):\n", __TOKEN_FMT(expr->as.unary.operator));
      break;
    case EXPR_BINARY:
      fprintf(stream, "binary (%.*s):\n", __TOKEN_FMT(expr->as.binary.operator));
      break;
    case EXPR_DECLARATION:
      fprintf(stream, "declaration (%.*s):\n", __TOKEN_FMT(expr->as.declaration.identifier));
      break;
    case EXPR_IDENTIFIER:
      fprintf(stream, "identifier '%.*s'\n", __TOKEN_FMT(expr->as.identifier));
      break;
    case EXPR_BLOCK:
      fprintf(stream, "block (count %zu):\n", expr->as.block.count);
      break;
  }
}

void dump_ast(Ast *ast, FILE *stream)
{
  ast_walk(ast, dump_expr, stream);
}
