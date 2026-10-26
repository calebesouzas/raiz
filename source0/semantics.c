#include "libc.h"
#include "generics.h"
#include "ast.h"
#include "report.h"

#define MESSAGE_SIZE 256
typedef struct {
  size_t error_id;
  bool error : 1;
  bool poisoned : 1;
} Result;

typedef array_of(Result) ResultStack;

typedef struct {
  char message[MESSAGE_SIZE];
  size_t node_id;
  Span *span;
} Error;

typedef array_of(Error) ErrorArray;

void check_node(Expr *node, uint32_t depth, void *data, void *out)
{
  ResultStack *result_stack = (ResultStack*) data;
  ErrorArray *errors = (ErrorArray*) out;

  for (uint32_t i = 0; i < depth; i++)
  {
    putc(' ', stderr);
  }

  const Result ok = {0};

  fprintf(stderr, "[%zu] checking (%d)\n", node->id, node->type);

  switch (node->type)
  {
    case EXPR_LITERAL:
    case EXPR_GROUP:
    case EXPR_UNARY:
    case EXPR_BLOCK:
      array_push(result_stack, ok);
      return;
    case EXPR_BINARY:
    {
      Result right = array_pop(result_stack);
      Result left = array_pop(result_stack);
      if (right.error || left.error)
      {
        return;
      }
    } break;
    case EXPR_IDENTIFIER:
    {
    } break;
    case EXPR_DECLARATION:
    {
    } break;
  }
}

bool check_ast(Ast *ast)
{
  ResultStack result_stack = {0};
  ErrorArray errors = {0};

  ast_walk(ast, check_node, true, &result_stack, &errors);

  array_for(Error, error, &errors)
  {
    report(error->span, "%s\n", error->message);
  }

  return errors.count == 0;
}
