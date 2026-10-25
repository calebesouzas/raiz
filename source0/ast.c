#include "libc.h"
#include "ast.h"

void expr_walk(Expr *pool, Expr *node, uint32_t depth, WalkerFunction visit, void *data, void *out)
{
  visit(node, depth, data, out);

  if (node == NULL)
    return;

  switch (node->type)
  {
    case EXPR_LITERAL:
    case EXPR_IDENTIFIER:
      break;
    case EXPR_GROUP:
      expr_walk(pool, &pool[node->as.group.inner_id], depth + 1, visit, data, out);
      break;
    case EXPR_UNARY:
      expr_walk(pool, &pool[node->as.unary.inner_id], depth + 1, visit, data, out);
      break;
    case EXPR_BINARY:
      expr_walk(pool, &pool[node->as.binary.left_id], depth + 1, visit, data, out);
      expr_walk(pool, &pool[node->as.binary.right_id], depth + 1, visit, data, out);
      break;
    case EXPR_DECLARATION:
      expr_walk(pool, &pool[node->as.declaration.value_id], depth + 1, visit, data, out);
      break;
    case EXPR_BLOCK:
      array_for(size_t, term_id, &node->as.block)
      {
        expr_walk(pool, &pool[*term_id], depth + 1, visit, data, out);
      }
      break;
  }
}

void ast_walk(Ast *ast, WalkerFunction visit, void *data, void *out)
{
  expr_walk(ast->pool.data, ast->root, 0, visit, data, out);
}

#define INDENT 2
void dump_expr(Expr *expr, uint32_t level, void *data, void *out)
{
  (void) out; // we're not "returning" anything

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
  ast_walk(ast, dump_expr, stream, NULL);
}
