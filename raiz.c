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
  Rz_StringValueMap scope = {0};
  while (fgets(buffer, sizeof(buffer), stdin))
  {
    if (strncmp(buffer, "exit", 4) == 0) break;

    Rz_ExprArena arena = {0};
    (void)rz_parser_build_ast(&arena, buffer);

    Rz_VM vm = rz_vm_new(&arena, &scope);

    Rz_Value result = rz_eval_arena(&vm);
    switch (result.kind)
    {
      case RZ_VALUE_INT:    printf("%d", result.as.integer); break;
      case RZ_VALUE_FLOAT:  printf("%f", result.as.floating); break;
      case RZ_VALUE_BOOL:   printf("%s", result.as.boolean ? "true" : "false"); break;
      case RZ_VALUE_CHAR:   printf("%c", result.as.character); break;
      case RZ_VALUE_STRING: printf("%.*s", result.as.string.size, result.as.string.data); break;
      case RZ_VALUE_VOID:   printf("void"); break;
      default:
        char buffer[64] = {0};
        sprintf(buffer, "value kind (%s)", rz_value_extract_kind(result));
        RZ_UNREACHABLE(buffer);
    }
    printf("\n");

    free(arena.items);
    printf("$> ");
  }
  return 0;
}
