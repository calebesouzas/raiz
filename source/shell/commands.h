#pragma once

#include <errno.h>
#include <unistd.h>

typedef struct
{
  char **argv;
  int argc;
  uint32_t argv_capacity;
} Raiz_Cmd;

bool raiz_cmd_run(Raiz_Cmd *command);

#define raiz_cmd_add(cmd, ...) raiz_cmd_add_((cmd), __VA_ARGS__, (char*)NULL)
void raiz_cmd_add_(Raiz_Cmd *command, ...);

/*@todo
 * Implement wrappers for subprocess (command) spawning.
 * We must have all basic features, like:
 * - [ ] redirecting stdin, stdout and stderr;
 * - [ ] getting exit or signal status and exit code if exited "normally";
 * - [ ] choose between running asyncronously or not;
 * - [ ] piping multiple commands (passing one's stdout into other's stdin);
 */
