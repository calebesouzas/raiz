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
  uint32_t indent_level;
} State;

static inline Result *push_result(State *state)
{
  Result result = {0};
  result.id = state->register_count++;
  array_push(&state->result_stack, result);
  return array_last(&state->result_stack);
}

__attribute__((format(printf, 2, 3)))
static inline void print(State *state, const char *format, ...)
{
  for (uint32_t i = 0; i < state->indent_level * 2; i++)
  {
    fputc(' ', state->stream);
  }
  va_list args;
  va_start(args, format);
  vfprintf(state->stream, format, args);
  va_end(args);
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
    case EXPR_DECLARATION:
      break;
    case EXPR_BLOCK:
    {
      scope_new(&state->scope_stack);
      state->indent_level++;
    } break;
    case EXPR_IF:
    {
      Result *result = push_result(state);
      print(state, "int r%u;\n", result->id);
    } break;
  }
}

void transpile_node_stage_pre(Expr *node, uint32_t stage, void *data, void *out)
{
  (void) out;

  State *state = (State*) data;

  switch (node->type)
  {
    case EXPR_LITERAL:
    case EXPR_GROUP:
    case EXPR_UNARY:
    case EXPR_BINARY:
    case EXPR_IDENTIFIER:
    case EXPR_BLOCK:
    case EXPR_DECLARATION:
    case EXPR_ASSIGN:
      break;
    case EXPR_IF:
    {
      switch (stage)
      {
        case WALK_STAGE_CONDITION:
          break; // insert "if (...)" at 'stage_post()' function
        case WALK_STAGE_THEN:
          print(state, "{\n");
          break;
        case WALK_STAGE_ELSE:
          print(state, "else\n  {\n");
          break;
        default:
          fprintf(stderr, "unreachable 'if' node stage: %u\n", stage);
          abort();
      }
    } break;
  }
}

void transpile_node_stage_post(Expr *node, uint32_t stage, void *data, void *out)
{
  (void) out;

  State *state = (State*) data;

  switch (node->type)
  {
    case EXPR_LITERAL:
    case EXPR_GROUP:
    case EXPR_UNARY:
    case EXPR_BINARY:
    case EXPR_IDENTIFIER:
    case EXPR_BLOCK:
    case EXPR_DECLARATION:
    case EXPR_ASSIGN:
      break;
    case EXPR_IF:
    {
      switch (stage)
      {
        case WALK_STAGE_CONDITION:
        {
          Result condition = array_pop(&state->result_stack);
          print(state, "if (r%u)\n", condition.id);
        } break;
        case WALK_STAGE_THEN:
        case WALK_STAGE_ELSE:
        {
          Result last = array_pop(&state->result_stack);
          Result *result = array_last(&state->result_stack);
          print(state, "r%u = r%u;\n  }\n", result->id, last.id);
        } break;
        default:
          fprintf(stderr, "unreachable 'if' node stage: %u\n", stage);
          abort();
      }
    } break;
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
      print(state, "int r%u = %u;\n", result->id, node->as.literal.value);
    } break;
    case EXPR_GROUP:
    case EXPR_IF:
      break;
    case EXPR_UNARY:
    {
      Result inner = array_pop(&state->result_stack);
      Result *result = push_result(state);
      print(state, "int r%u = -r%u;\n", result->id, inner.id);
    } break;
    case EXPR_BINARY:
    {
      Result right = array_pop(&state->result_stack);
      Result left = array_pop(&state->result_stack);
      Result *result = push_result(state);
      print(state,
          "int r%u = r%u %.*s r%u;\n",
          result->id, left.id, __TOKEN_FMT(node->as.binary.operator), right.id
      );
    } break;
    case EXPR_ASSIGN:
    {
      Result right = array_pop(&state->result_stack);
      const Result *left = array_last(&state->result_stack);
      print(state, "r%u = r%u;\n", left->id, right.id);
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
      state->indent_level--;
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

  print(&state, "#include <stdio.h>\nint main(void){\n");
  WalkerCombo combo = {
    .pre = transpile_node_pre,
    .post = transpile_node_post,
    .stage_pre = transpile_node_stage_pre,
    .stage_post = transpile_node_stage_post
  };

  state.indent_level++;

  Result last_result = {0};
  ast_walk_combo(ast, &combo, &state, &last_result);

  print(&state, "printf(\"%%d\\n\", r%u);\n", last_result.id);
  print(&state, "return 0;\n}\n");

  state.indent_level--;

  fclose(state.stream);
  array_for(Scope, scope, &state.scope_stack)
  {
    array_free(&scope->symbols);
  }
  array_free(&state.scope_stack);
  return true;
}
