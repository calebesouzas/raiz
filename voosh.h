#pragma once

typedef enum {
  VOOSH_CMD_INVALID = 0,
  VOOSH_CMD_BUILD,
  VOOSH_CMD_NONE,
  VOOSH_CMD_RUN,
  VOOSH_CMD_TEST,
  __voosh_cmd_count,
} Voosh_Cmd;

typedef enum {
  VOOSH_FLAG_INVALID  = 0,
  VOOSH_FLAG_STRICT   = (1 << 0),
  VOOSH_FLAG_NO_REBUILD = (1 << 1),
  // @note can't use flag count for static asserts D:
} Voosh_Flag;

typedef struct {
  int argc;
  char **argv;
  Voosh_Cmd command;
  int flags;
  Raiz_CStrings raiz_args;
} Voosh;

void voosh_rebuild_urself(int argc, char **argv, char **envp);
