#pragma once

#include <unistd.h>

/**
 * @doc!
 * @name(header) shell/commands.h
 * @tags helper, process
 * @desc declares public types and functions for spawning other processes
 */

/**
 * @doc!
 * @name(macro) CMD_RUN_UNIX_VA
 * @tags helper, process
 * @params command, ...
 * @require [@errors/panics.h], <string.h> and <errno.h>
 * @desc runs `command` with `...` arguments just like in the shell.
 * @note `command` has to be in $PATH
 * @note(safety) `command` is evaluated once
 */
#define RAIZ_CMD_RUN_UNIX_VA(command, ...)\
  do {\
    char *__cmd = (command);\
    pid_t pid = fork();\
    if (pid == 0) {\
      execlp(__cmd, __cmd, __VA_ARGS__, (char*)NULL);\
    } else if (pid < 0) {\
      RAIZ_PANIC("failed to spawn child process: %s. %s\n",\
          __cmd, strerror(errno));\
    }\
  } while (0)

/**
 * @doc!
 * @name(macro) CMD_RUN_UNIX_VEC
 * @tags helper, process
 * @params command, argv
 * @require [@errors/panics.h], <string.h> and <errno.h>
 * @desc runs `command` with `argv` arguments just like in the shell.
 * @note `command` has to be in $PATH
 * @note `argv` has to be entirelly null-terminated
 * @note(safety) `command` is evaluated once
 */
#define RAIZ_CMD_RUN_UNIX_VEC(command, argv)\
  do {\
    char *__cmd = (command);\
    pid_t pid = fork();\
    if (pid == 0) {\
      execvp(__cmd, (argv));\
    } else if (pid < 0) {\
      RAIZ_PANIC("failed to spawn child process: %s. %s\n",\
          __cmd, strerror(errno));\
    }\
  } while (0)
