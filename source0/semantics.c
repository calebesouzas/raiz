#include "semantics.h"
#include "symbols.h"
#include "report.h"

#define MESSAGE_SIZE 256
typedef struct {
  size_t error_id;
  bool error : 1;
  bool poisoned : 1;
} Result;

typedef array_of(Result) ResultStack;

typedef struct {
  SymbolArray symbols;
} Scope;

typedef struct {
  Scope scope;
  ResultStack result_stack;
} State;

typedef struct {
  char message[MESSAGE_SIZE];
  size_t node_id;
  Span *span;
} Error;

typedef array_of(Error) ErrorArray;

Symbol *scope_search(Scope *scope, const char *name, size_t length)
{
  array_for(Symbol, symbol, &scope->symbols)
  {
    if (symbol->span.length == length && strncmp(symbol->span.text, name, length) == 0)
    {
      return symbol;
    }
  }
  return NULL;
}

Symbol *scope_insert(Scope *scope, Span *span, size_t value_id)
{
  Symbol symbol = {0};
  symbol.value_id = value_id;
  symbol.poisoned = false;
  symbol.span = *span;
  array_push(&scope->symbols, symbol);
  return array_last(&scope->symbols);
}

static inline Result push_error(ErrorArray *errors, Expr *node, const char *format, ...)
{
  Error error = {
    .message = {0},
    .node_id = node->id,
    .span = &node->span,
  };

  va_list args;
  va_start(args, format);
  vsnprintf(error.message, sizeof(error.message), format, args);
  va_end(args);

  array_push(errors, error);

  return (Result)
  {
    .error_id = errors->count - 1,
    .error = true,
    .poisoned = true,
  };
}

void check_node(Expr *node, uint32_t depth, void *data, void *out)
{
  (void) depth;

  State *state = (State*) data;
  ErrorArray *errors = (ErrorArray*) out;

  const Result ok = {0};
  const Result poison = {.poisoned = true, .error = false, .error_id = 0};
  // error resuls are generated dynamically
  switch (node->type)
  {
    case EXPR_LITERAL:
    case EXPR_GROUP:
      array_push(&state->result_stack, ok);
      return;
    case EXPR_UNARY:
    {
      Result inner = array_pop(&state->result_stack);
      if (inner.poisoned)
      {
        array_push_no_resize(&state->result_stack, poison);
        return;
      }

      array_push_no_resize(&state->result_stack, ok);
    } break;
    case EXPR_BLOCK:
    {
      bool poisoned = false;
      for (size_t i = 0; i < node->as.block.count; i++)
      {
        Result term = array_pop(&state->result_stack);
        poisoned = poisoned || term.poisoned;
      }

      const Result *result = poisoned? &poison : &ok;
      if (node->as.block.count == 0)
      {
        array_push(&state->result_stack, *result);
      }
      else
      {
        array_push_no_resize(&state->result_stack, *result);
      }
    } break;
    case EXPR_BINARY:
    {
      Result right = array_pop(&state->result_stack);
      Result left = array_pop(&state->result_stack);
      if (right.poisoned || left.poisoned)
      {
        array_push_no_resize(&state->result_stack, poison);
        return;
      }

      array_push_no_resize(&state->result_stack, ok);
    } break;
    case EXPR_IDENTIFIER:
    {
      Token *identifier = node->as.identifier;
      Symbol *symbol = scope_search(&state->scope, identifier->span.text, identifier->span.length);
      if (symbol == NULL)
      {
        push_error(errors, node, "use of undeclared identifier '%.*s'", __TOKEN_FMT(identifier));
        array_push(&state->result_stack, poison);

        // do not castade the error further!
        // TODO: system to actually allow some repeated "undeclared identifier" errors but with a
        // carefully set limit.
        symbol = scope_insert(&state->scope, &identifier->span, node->id);
        symbol->poisoned = true;
        return;
      }
      else if (symbol->poisoned)
      {
        array_push(&state->result_stack, poison);
        return;
      }

      array_push(&state->result_stack, ok);
    } break;
    case EXPR_DECLARATION:
    {
      Result value = array_pop(&state->result_stack);
      if (value.poisoned)
      {
        array_push_no_resize(&state->result_stack, poison);
        return;
      }

      Token *identifier = node->as.declaration.identifier;
      Symbol *symbol = scope_search(&state->scope, identifier->span.text, identifier->span.length);
      // TODO: think on what to do with redeclaration of poisoned symbols
      if (symbol != NULL)
      {
        Result error = push_error(errors, node,
            "redeclaration of symbol '%.*s'", __TOKEN_FMT(identifier)
        );
        array_push_no_resize(&state->result_stack, error);
        return;
      }

      scope_insert(&state->scope, &node->span, node->as.declaration.value_id);
      array_push_no_resize(&state->result_stack, ok);
    } break;
  }
}

bool check_ast(Ast *ast)
{
  State state = {0};
  ErrorArray errors = {0};

  ast_walk(ast, check_node, true, &state, &errors);

  array_for(Error, error, &errors)
  {
    report(error->span, "%s\n", error->message);
  }

  return errors.count == 0;
}
