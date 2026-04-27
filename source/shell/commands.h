#pragma once

#include <errno.h>
#include <unistd.h>

#define RAIZ_CMD_RUN_UNIX_VA(command, ...)\
  do\
  {\
    char *__cmd = (command);\
    pid_t pid = fork();\
    if (pid == 0)\
    {\
      execlp(__cmd, __cmd, __VA_ARGS__, (char*)NULL);\
    }\
    else if (pid < 0)\
    {\
      RAIZ_PANIC("failed to spawn child process: %s. %s\n",\
          __cmd, strerror(errno));\
    }\
  } while (0)

#define RAIZ_CMD_RUN_UNIX_VEC(command, argv)\
  do\
  {\
    char *__cmd = (command);\
    pid_t pid = fork();\
    if (pid == 0)\
    {\
      execvp(__cmd, (argv));\
    }\
    else if (pid < 0)\
    {\
      RAIZ_PANIC("failed to spawn child process: %s. %s\n",\
          __cmd, strerror(errno));\
    }\
  } while (0)

typedef struct
{
  char **argv;
  int argc;
  uint32_t argv_capacity;
} Raiz_Cmd;

bool raiz_cmd_run(Raiz_Cmd *command);

#define raiz_cmd_add(cmd, ...) raiz_cmd_add_((cmd), __VA_ARGS__, (char*)NULL)
void raiz_cmd_add_(Raiz_Cmd *command, ...);
