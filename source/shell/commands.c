#include <stdarg.h>

#include "commands.h"
#include "common/arrays.h"
#include "common/libc.h"
#include "errors/assert.h"

void
cmd_new(Raiz_Cmd *command)
{
  command->argv_capacity = RAIZ_DA_INITIAL_CAPACITY;
  command->argv = malloc(command->argv_capacity);
  RAIZ_ASSERT(command->argv, "failed to allocate command argv buffer\n");
}

void
raiz_cmd_add_(Raiz_Cmd *command, ...)
{
  if (command->argv_capacity == 0)
  {
    cmd_new(command);
  }

  va_list args;
  va_start(args, command);

  char *arg = NULL;
  for (int i = 0; (arg = va_arg(args, char*)) != NULL; i++)
  {
    command->argv[command->argc++] = arg;
  }
}
