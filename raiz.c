#include "common.h"
#include "maps.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "runtime.h"

int main(void)
{
  char buffer[1024] = {0};

  printf("$> ");
  Rz_StringDoubleMap scope = {0};
  while (fgets(buffer, sizeof(buffer), stdin))
  {
    if (strncmp(buffer, "exit", 4) == 0) break;

    Rz_ExprArena arena = {0};
    (void)rz_parser_build_ast(&arena, buffer);

    Rz_VM vm = rz_vm_new(&arena, &scope);

    printf("Result %f\n", rz_eval_arena(&vm));

    free(arena.items);
    printf("$> ");
  }
  return 0;
}
