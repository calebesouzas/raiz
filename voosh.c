#include <errno.h>
#include "source/common/libc.h"
#include "source/common/arrays.h"
#include "source/errors/panics.h"
#include "source/shell/commands.h"
#include "voosh.h"

/**
 * @doc!
 * @name(function) main
 * @desc the entry point
 * @param `int argc` CLI argument count (including the program name)
 * @param `char **argv` argument vector, raw strings
 * @param `char **envp` environment variables. Also raw strings.
 * @note `envp` items syntax: `VAR=value` where value must be parsed sometimes.
 */
int
main(int argc, char **argv, char **envp) {
  voosh_rebuild_urself(argc, argv, envp);

  Raiz_CStrings raiz_args = {0};
  Voosh_Cmd command = voosh_parse_args(argc, argv, &raiz_args);

  switch (command.kind) {
  case VOOSH_CMD_RUN:   voosh_run(command.flags, raiz_args.items); break;
  case VOOSH_CMD_BUILD: voosh_build(command.flags); break;
  default: RAIZ_PANIC("voosh: invalid command (index: %d)\n", command.kind);
  }

  return 0;
}

static int  parse_flags  (Voosh_Cmd *cmd, char *arg);
static bool parse_option (Voosh_Cmd *cmd, char *arg);
static bool parse_command(Voosh_Cmd *cmd, char *arg);
static void parse_raiz_args(char **argv, Raiz_CStrings *args);

Voosh_Cmd
voosh_parse_args(int argc, char **argv, char ***raiz_args) {
  Voosh_Cmd cmd = {0};

  for (int i = 1; i < argc; ++i) {
    char *arg = argv[i];
    int err_index = 0;

    // @todo don't nest!
    if (arg[0] == '-') {
      if (arg[1] == '-') {
        if (arg[2] == '\0') {
          parse_raiz_args(argc - i, argv + i, raiz_args);
          break;
        } else if (!parse_option(&cmd, arg)) {
          RAIZ_PANIC("voosh: invalid option: '--%s'\n", arg);
        }
      } else if ((err_index = parse_flags(&cmd, arg)) != 0) {
        RAIZ_PANIC("voosh: invalid flag: -%c\n", arg[err_index]);
      }
    } else if (!parse_command(&cmd, arg)) {
      RAIZ_PANIC("voosh: invalid command: '%s'\n", arg);
    }
  }

  return cmd;
}

/**
 * @doc!
 * @name(function) parse_long_arg
 * @param `Cmd *cmd` mutable pointer to command to mutate `flags` field.
 * @param `char *arg` single argument from `argv` (starting with single '-')
 * @return `int` where non-zero means an invalid flag was found.
 * When it happens, the value returned is the index of wrong flag in `arg`
 */
static int
parse_flags(Voosh_Cmd *cmd, char *arg) {
  size_t len = strlen(arg);
  for (size_t i = 1; i < len; ++i) {
    switch (arg[i]) {
    case 's': {
      cmd->flags |= VOOSH_FLAG_STRICT;
    } break;
    default: return (int) i;
    }
  }

  return 0;
}

/**
 * @doc!
 * @name(function) parse_option
 * @params same as [@parse_flags]
 * @desc parses a simple option from `arg` (starting with "--")
 * @return `bool` where `false` means failure
 */
static bool
parse_option(Voosh_Cmd *cmd, char *arg) {
  arg += 2; // the pointer is local to this function's stack
  if (strcmp(arg, "no-build") == 0) cmd->flags |= VOOSH_FLAG_NO_BUILD;
  else if (strcmp(arg, "strict") == 0) cmd->flags |= VOOSH_FLAG_STRICT;
  else return false;

  return true;
}

/**
 * @doc!
 * @name(function) parse_command
 * @params same as [@parse_flags]
 * @desc parses a command from `arg` (not starting with any dashes)
 * @return `bool`, `false` for failure
 */
static bool
parse_command(Voosh_Cmd *cmd, char *arg) {
  if (strcmp(arg, "run") == 0) cmd->kind = VOOSH_CMD_RUN;
  else if (strcmp(arg, "build") == 0) cmd->kind = VOOSH_CMD_BUILD;
  else return false;

  return true;
}

static void
parse_raiz_args(int argc, char **argv, Raiz_CStrings *args) {
  for (int i = 0; i < argc; ++i) {
    raiz_da_append(args, argv[i]);
  }
}

// @todo support other compilers than Clang
// @todo rebuild only when needed
void
voosh_rebuild_urself(int argc, char **argv, char **envp) {
  // @todo use environment variables
  (void) argc;
  (void) envp;

  char *binary_path = *argv++;
  char *source_path = __FILE__;

  RAIZ_CMD_RUN_UNIX_VA("clang", "-o", binary_path, source_path);

  RAIZ_CMD_RUN_UNIX_VEC(binary_path, argv);

  // we have to exit, to prevent infinite recursion with process spawning
  exit(0);
}

bool
is_c_file(const char *path) {
  size_t path_len = strlen(path);
  if (!path_len > 2) RAIZ_PANIC("voosh: path %s is too short\n", path);
  return path[path_len - 2] == '.' && path[path_len - 1] == 'c';
}

void
get_source_paths_func(const char *path, mode_t mode, void *raw_data) {
  Raiz_CStrings *strings = raw_data;
  if (S_ISREG(mode) && is_c_file(path)) raiz_da_append(strings, path);
}

void
voosh_build(int flags) {
  char **compiler_argv = {
    "clang",
    "-x", "c",
    "-o", "./build/raiz",
    "-Wall", "-Wextra",
  };

  char **strict_flags = {
    "-Werror",
    "-Wpedantic", "-pedantic",
    "-fsanatize=address,memory,undefined"
  };
  size_t strict_flags_count = sizeof(strict_flags)/sizeof(strict_flags[0]);


  Raiz_CStrings cmd = {0};

  for (size_t i = 0; i < cmd.count; ++i) {
    raiz_da_append(&cmd, compiler_argv[i]);
  }

  if (!raiz_dir_walk_recursive("./source", get_source_paths_func, &cmd)) {
    RAIZ_PANIC("voosh: failed to open source folder:\n%s\n", strerror(errno));
  }


  if (flags & VOOSH_FLAG_STRICT) {
    for (size_t i = cmd.count; i - cmd.count < strict_flags_count; ++i) {
      raiz_da_append(&cmd, strict_flags[i]);
    }
  }

  RAIZ_CMD_RUN_UNIX_VEC("clang", cmd.items);
}

void
voosh_run(int flags, char **argv) {
  (void) flags;

  RAIZ_CMD_RUN_UNIX_VEC("./build/raiz", argv);
}
