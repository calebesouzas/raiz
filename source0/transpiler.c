#include "libc.h"
#include "ast.h"
#include "scope.h"

typedef struct {
  FILE *stream;
  ScopeStack *scope_stack;
} State;

void transpile_node_pre(Expr *node, uint32_t depth, void *data, void *out)
{
  (void) depth;
  (void) out;

  State *state = (State*) data;

  switch (node->type)
  {
    case EXPR_LITERAL:
      break;
    case EXPR_GROUP:
      break;
    case EXPR_UNARY:
      break;
    case EXPR_BINARY:
      break;
    case EXPR_IDENTIFIER:
      break;
    case EXPR_BLOCK:
      break;
    case EXPR_DECLARATION:
      break;
  }
}

void transpile_node_post(Expr *node, uint32_t depth, void *data, void *out)
{
  (void) depth;
  (void) out;

  State *state = (State*) data;

  switch (node->type)
  {
    case EXPR_LITERAL:
      break;
    case EXPR_GROUP:
      break;
    case EXPR_UNARY:
      break;
    case EXPR_BINARY:
      break;
    case EXPR_IDENTIFIER:
      break;
    case EXPR_BLOCK:
      break;
    case EXPR_DECLARATION:
      break;
  }
}

bool transpile_ast(Ast *ast, ScopeStack *scope_stack, const char *output_path)
{
  if (output_path == NULL)
  {
    output_path = "./main.c";
  }

  State state = {0};
  state.stream = fopen(output_path, "w");
  if (state.stream == NULL)
  {
    fprintf(stderr, "raiz: failed to open '%s': %s\n", output_path, strerror(errno));
    return false;
  }

  state.scope_stack = scope_stack;

  WalkerCombo combo = {
    .pre = transpile_node_pre,
    .post = transpile_node_post
  };

  ast_walk_combo(ast, &combo, &state, NULL);
  return true;
}
