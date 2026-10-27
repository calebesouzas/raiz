#include "libc.h"
#include "files.h"
#include "ast.h"
#include "parser.h"
#include "semantics.h"

bool process_program(const char *source, size_t length, const char *file_path);

int main(int argc, char **argv)
{
  if (argc <= 1)
  {
    fprintf(stderr, "usage: %s <raiz file>\n", argv[0]);
    return 1;
  }

  char *source = NULL;
  size_t size = 0;
  if (!read_entire_file(argv[1], &source, &size))
    return 1;

  if (!process_program(source, size, argv[1]))
  {
    fprintf(stderr, "raiz: process failed\n");
    return 1;
  }

  free(source);
  source = NULL;

  return 0;
}

bool process_program(const char *source, size_t length, const char *file_path)
{
  Ast ast = parse(source, length, file_path);
  if (!ast.ok)
    return false;

  bool result = true;
  // dump_ast(&ast, stderr);
  if (!check_ast(&ast))
    result = false;

  array_free(&ast.pool);
  return result;
}
