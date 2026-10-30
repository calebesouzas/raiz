#include "transpiler.h"

typedef enum {
  ID_TEMP = 't',
  ID_VAR = 'v',
} IdType;

typedef struct {
  IdType type;
  union {
    uint32_t any_id;
    uint32_t temp_id;
    uint32_t var_id;
  } as;
} Result;

typedef array_of(Result) ResultStack;

typedef struct {
  FILE *stream;
  ScopeStack *scope_stack;
  ResultStack result_stack;
  uint32_t temp_count;
  uint32_t var_count;
} State;

static inline Result *push_temp_result(State *state)
{
  Result result = {0};
  result.type = ID_TEMP;
  result.as.temp_id = state->temp_count++;
  array_push(&state->result_stack, result);
  return array_last(&state->result_stack);
}

static inline Result *push_var_result(State *state)
{
  Result result = {0};
  result.type = ID_VAR;
  result.as.var_id = state->var_count++;
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
    case EXPR_GROUP:
      break;
    case EXPR_UNARY:
    {
      Result inner = array_pop(&state->result_stack);
      Result *result = push_temp_result(state);
      fprintf(state->stream,
          "  int t_%d = -%c_%d;\n", result->as.temp_id, inner.type, inner.as.any_id
      );
    } break;
    case EXPR_BINARY:
    {
      Result right = array_pop(&state->result_stack);
      Result left = array_pop(&state->result_stack);
      Result *result = push_temp_result(state);
      fprintf(state->stream,
          "  int t_%d = %c_%d + %c_%d;\n",
          result->as.temp_id, left.type, left.as.any_id, right.type, right.as.any_id
      );
    } break;
    case EXPR_IDENTIFIER:
      break;
    case EXPR_BLOCK:
      break;
    case EXPR_DECLARATION:
      break;
  }
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

  WalkerCombo combo = {
    .pre = transpile_node_pre,
    .post = transpile_node_post
  };

  ast_walk_combo(ast, &combo, &state, NULL);
  fclose(state.stream);
  return true;
}
