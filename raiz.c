#include "common.h"
#include "lexer.c"
#include "parser.c"
#include "runtime.c"

int main(void)
{
  char buffer[1024] = {0};

  printf("$> ");
  while (fgets(buffer, sizeof(buffer), stdin))
  {
    if (strncmp(buffer, "exit", 4) == 0) break;

    Rz_ExprArena arena = {0};
    (void)rz_parser_build_ast(&arena, buffer);

    Rz_VM vm = rz_vm_new(&arena);

    printf("Result %d\n", rz_eval_arena(&vm));

    free(arena.items);
    printf("$> ");
  }
  return 0;
}
