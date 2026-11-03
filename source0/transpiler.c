#include "transpiler.h"

typedef struct {
  uint32_t id;
} Result;

typedef array_of(Result) ResultStack;

typedef struct {
  FILE *stream;
  ScopeStack scope_stack;
  ResultStack result_stack;
  uint32_t register_count;
} State;

static inline Result *push_result(State *state)
{
  Result result = {0};
  result.id = state->register_count++;
  array_push(&state->result_stack, result);
  return array_last(&state->result_stack);
}

void transpile_node_pre(Expr *node, uint32_t depth, void *data, void *out)
{
  (void) depth;
  (void) out;

  State *state = (State*) data;

  switch (node->type)
  {
    case EXPR_LITERAL:
    case EXPR_GROUP:
    case EXPR_UNARY:
    case EXPR_BINARY:
    case EXPR_ASSIGN:
    case EXPR_IDENTIFIER:
      break;
    case EXPR_BLOCK:
    {
      scope_new(&state->scope_stack);
    } break;
    case EXPR_DECLARATION:
      break;
  }
}

void transpile_node_post(Expr *node, uint32_t depth, void *data, void *out)
{
  (void) depth;

  State *state = (State*) data;

  switch (node->type)
  {
    case EXPR_LITERAL:
    {
      Result *result = push_result(state);
      fprintf(state->stream, "  int r%d = %d;\n", result->id, node->as.literal.value);
    } break;
    case EXPR_GROUP:
      break;
    case EXPR_UNARY:
    {
      Result inner = array_pop(&state->result_stack);
      Result *result = push_result(state);
      fprintf(state->stream, "  int r%d = -r%d;\n", result->id, inner.id);
    } break;
    case EXPR_BINARY:
    {
      Result right = array_pop(&state->result_stack);
      Result left = array_pop(&state->result_stack);
      Result *result = push_result(state);
      fprintf(state->stream,
          "  int r%d = r%d %.*s r%d;\n",
          result->id, left.id, __TOKEN_FMT(node->as.binary.operator), right.id
      );
    } break;
    case EXPR_ASSIGN:
    {
      Result right = array_pop(&state->result_stack);
      const Result *left = array_last(&state->result_stack);
      fprintf(state->stream, "  r%d = r%d;\n", left->id, right.id);
    } break;
    case EXPR_IDENTIFIER:
    {
      Symbol *symbol = scope_search(&state->scope_stack, node->span.text, node->span.length);
      Result *result = push_result(state);
      result->id = symbol->value_id;
    } break;
    case EXPR_BLOCK:
    {
      array_pop(&state->scope_stack);
    } break;
    case EXPR_DECLARATION:
    {
      Result *value = array_last(&state->result_stack);
      scope_insert(&state->scope_stack, &node->span, value->id);
    } break;
  }

  Result *last_result = (Result*) out;
  (*last_result) = *array_last(&state->result_stack);
}

bool transpile_ast(Ast *ast, const char *output_path)
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

  scope_new(&state.scope_stack);

  fprintf(state.stream, "#include <stdio.h>\nint main(void){\n");
  WalkerCombo combo = {
    .pre = transpile_node_pre,
    .post = transpile_node_post
  };

  Result last_result = {0};
  ast_walk_combo(ast, &combo, &state, &last_result);

  fprintf(state.stream, "  printf(\"%%d\\n\", r%d);\n", last_result.id);
  fprintf(state.stream, "  return 0;\n}\n");

  fclose(state.stream);
  array_for(Scope, scope, &state.scope_stack)
  {
    array_free(&scope->symbols);
  }
  array_free(&state.scope_stack);
  return true;
}
