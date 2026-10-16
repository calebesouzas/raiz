#include "source0/libc.h"
#include "source0/files.h"
#include "source0/lexer.h"
#include "source0/ast.h"
#include "source0/parser.h"

bool test_lexer(const char *source, const size_t size)
{
  Lexer lexer = {
    .source = source,
    .size = size,
    .cursor = source,
    .remaining = size,
    .column = 1,
    .line_count = 1,
  };

  Token token = {0};
  printf("[\n");
  do
  {
    token = next_token(&lexer);
    printf("  [%zu](%zu:%zu):", token.location.index, token.location.line, token.location.column);
    printf(" \"%.*s\"", __TOKEN_FMT(&token));

    if (token.type == TOKEN_NUMBER)
    {
      printf(" %d", token.as.literal);
    }
    printf("\n");
  } while (token.type != TOKEN_END_OF_FILE);
  printf("]\n");

  return true;
}

bool test_parser(const char *source, const size_t size)
{
  Ast ast = parse(source, size);
  if (ast.root != NULL)
  {
    dump_ast(&ast, stderr);
  }
  else if (ast.arena == NULL)
  {
    return false;
  }

  free_expr_arena(ast.arena);
  ast.arena = NULL;

  return true;
}

typedef bool (*TestFunction) (const char *source, const size_t size);

int main(void)
{
  char *source = NULL;
  size_t size = 0;
  if (!read_entire_file("test.raiz", &source, &size))
    return 1;

  const TestFunction test_functions[] = {
    test_lexer,
    test_parser,
  };
  const char *test_names[] = {
    "token list",
    "AST dump",
  };

  _Static_assert(
      sizeof(test_functions)/sizeof(test_functions[0]) == sizeof(test_names)/sizeof(test_names[0]),
      "Should keep test functions and names in sync!"
  );

  unsigned int total = 0;
  unsigned int success = 0;
  unsigned int failure = 0;

  for (unsigned int index = 0; index < sizeof(test_functions)/sizeof(test_functions[0]); index++)
  {
    printf("// TESTING: %s\n", test_names[index]);
    if (test_functions[index](source, size))
    {
      success++;
    }
    else
    {
      failure++;
    }
    total++;
  }

  printf("resume: successful: %u, failed: %u, total: %u\n", success, failure, total);

  return failure > 0;
}

#include "source0/files.c"
#include "source0/lexer.c"
#include "source0/token.c"
#include "source0/ast.c"
#include "source0/parser.c"
