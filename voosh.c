#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>
#include "source/common/libc.h"
#include "source/common/arrays.h"
#include "source/errors/panics.h"
// @todo don't depend on Raiz! It may cause cycling dependencies!!!
#include "source/strings/slice.c"
#include "source/strings/array.h"
#include "source/filesystem/dirs.c"
#include "source/shell/commands.h"
#include "voosh.h"

extern int execvpe(const char *path, char *const argv[], char *const envp[]);

#define streq(source, destine) (strcmp(source, destine) == 0)
#define strneq(source, destine, limit) (strncmp(source, destine, limit) == 0)

void
print_usage_guide(Voosh *voosh) {
  printf("voosh: usage (regex for syntax, inside <> have special meanings):\n"
         "\n"
         "$ %s <command>? (-[<flags>]|[--<options>(=<value>)?])\n"
         "\n"
         "Available <command>s:\n"
         "  build  will rebuild voosh if needed then build Raiz project\n"
         "  run    will just run the Raiz project\n"
         "  test   will build and run all tests inside ./tests/ folder\n"
         "\n"
         "As you probably noticed, passing a command is optional.\n"
         "\n"
         "Available <flag>s and <option>s:\n"
         "  -s, --strict  (used with 'build') will compile Raiz project with"
         " strict compiler flags (like '-Werror' and '-Wpedantic')\n",
         voosh->argv[0]);
}

void
report_invalid_command(Voosh *voosh, char *command) {
  fprintf(stderr, "voosh: invalid command '%s'\n", command);
  fprintf(stderr, "voosh: consider running '%s --help'\n", voosh->argv[0]);
  exit(1);
}

void
report_invalid_option_or_flag(Voosh *voosh, char *option_or_flag) {
  fprintf(stderr, "voosh: invalid option or flag '%s'\n", option_or_flag);
  fprintf(stderr, "voosh: consider running '%s --help'\n", voosh->argv[0]);
  exit(1);
}

void
handle_raiz_argv(Voosh *voosh, int raiz_args_start) {
  for (int i = raiz_args_start; voosh->argv[i] != NULL; i++) {
    raiz_da_append(&voosh->raiz_args, voosh->argv[i]);
  }
}

void
handle_options(Voosh *voosh, int current) {
  for (; current < voosh->argc; current++) {
    char *arg = voosh->argv[current];
    if (streq(arg, "--strict") || streq(arg, "-s")) {
      voosh->flags |= VOOSH_FLAG_STRICT;
    } else if (streq(arg, "--no-rebuild")) {
      voosh->flags |= VOOSH_FLAG_NO_REBUILD;
    } else if (streq(arg, "--")) {
      handle_raiz_argv(voosh, current);
      break;
    }
  }
}

void
handle_command(Voosh *voosh, char *arg) {
  if (streq(arg, "build")) voosh->command = VOOSH_CMD_BUILD;
  else if (streq(arg, "run")) voosh->command = VOOSH_CMD_RUN;
  else report_invalid_command(voosh, arg);
}

static inline bool
is_option_or_flag(char *arg) {
  return (arg[0] == '-');
}

void
command_none(Voosh *voosh) {
  print_usage_guide(voosh);
}

void
command_run(Voosh *voosh) {
  RAIZ_CMD_RUN_UNIX_VEC(voosh->raiz_args.items[0], voosh->raiz_args.items);
}

bool
is_c_file(char *path) {
  // @note we're sure that `strlen(path)` > 0
  return path[strlen(path) - 1] == 'c';
}

void
command_build(Voosh *voosh) {
  // @todo implement raiz_da_append_many

  char *static_compiler_command[] = {
    "clang",
    "-o", "./build/raiz",
    "-Wall", "-Wextra",
    "-I./source/",
    "./source/main.c",
  };

  char *strict_compiler_flags[] = {
    "-Werror", "-Wpedantic", "-pedantic",
    "fsanatize=address,memory,undefined",
  };

  Raiz_CStrings dynamic_compiler_command = {0};

  for (int i = 0; i < RAIZ_ARRAY_LEN(static_compiler_command); i++) {
    raiz_da_append(&dynamic_compiler_command, static_compiler_command[i]);
  }
  
  if (voosh->flags & VOOSH_FLAG_STRICT) {
    for (int i = 0; i < RAIZ_ARRAY_LEN(strict_compiler_flags); i++) {
      raiz_da_append(&dynamic_compiler_command, strict_compiler_flags[i]);
    }
  }

#if 1
  printf("voosh: running compiler command:\n $ ");
  for (size_t i = 0; i < dynamic_compiler_command.count; i++) {
    printf("%s ", dynamic_compiler_command.items[i]);
  }
  printf("\n");
#endif

  RAIZ_CMD_RUN_UNIX_VEC(dynamic_compiler_command.items[0],
                        dynamic_compiler_command.items);
}

void
command_test(Voosh *voosh) {
  Raiz_CStrings tests = raiz_dir_get_relative_file_paths("./tests");

  for (size_t current = 0; current < tests.count; current++) {
    Raiz_String c_file_path = raiz_str_from_cstr(tests.items[current]);

    char *bin_path = malloc(c_file_path.size);
    assert(bin_path != NULL);

    snprintf(bin_path, c_file_path.size, "./build/%.*s",
             c_file_path.size - 2, c_file_path.data);

    fprintf(stderr, "%s\n", bin_path);
    // RAIZ_CMD_RUN_UNIX_VA("clang", "-o", bin_path, "-I./source/", "-ggdb");
  }
}

void
run_voosh_command(Voosh *voosh) {
  static_assert(__voosh_cmd_count == 5, "Voosh_Cmd variants changed");

  switch (voosh->command) {
  case VOOSH_CMD_BUILD: command_build(voosh); break;
  case VOOSH_CMD_RUN:   command_run(voosh);   break;
  case VOOSH_CMD_TEST:  command_test(voosh);  break;
  case VOOSH_CMD_NONE:  command_none(voosh);  break;
  default: assert(0 && "voosh: reached unreachable command");
  }

  free(voosh->raiz_args.items);
}

int
main(int argc, char **argv, char **envp) {
  voosh_rebuild_urself(argc, argv, envp);

  Voosh voosh = {
    .argc = argc,
    .argv = argv,
    .command = argc > 1 ? VOOSH_CMD_INVALID : VOOSH_CMD_NONE,
    .flags = VOOSH_FLAG_INVALID,
    .raiz_args = NULL,
  };
  raiz_da_append(&voosh.raiz_args, "./build/raiz");

  for (int current = 1; current < argc; current++) {
    char *arg = argv[current];

    if (is_option_or_flag(arg)) {
      handle_options(&voosh, current);
    } else {
      handle_command(&voosh, arg);
    }
  }

  run_voosh_command(&voosh);

  return 0;
}

bool
needs_rebuild(char *binary_path, char *source_path) {
  struct stat binary_stat;
  stat(binary_path, &binary_stat);
  int binary_last_mod_time = binary_stat.st_mtime;
  // printf("binary: %d\n", binary_last_mod_time);

  struct stat source_stat;
  stat(source_path, &source_stat);
  int source_last_mod_time = source_stat.st_mtime;
  // printf("source: %d\n", source_last_mod_time);

  return source_last_mod_time > binary_last_mod_time
           && binary_last_mod_time != 0;
}

void
rebuild(char **argv, char **envp) {
  fprintf(stderr, "voosh: rebuilding myself\n");

  pid_t clang_pid = fork();
  if (clang_pid == 0) {
    // fprintf(stderr, "voosh: running compiler command\n");
    execlp("clang", "clang", "-o", "voosh", __FILE__, NULL);
  }

  // @todo maybe handle fork() possible failure
  if (clang_pid < 0) return; // fork failed

  int clang_status = 0;
  int options = WNOHANG;
  waitpid(clang_pid, &clang_status, options);

  if (!WIFEXITED(clang_status)) {
    fprintf(stderr, "voosh: compiler command failed, didn't exit\n");
    exit(2);
  }

  if (WEXITSTATUS(clang_status) != 0) {
    fprintf(stderr, "voosh: compiler command failed with exit code %d\n",
            WEXITSTATUS(clang_status));
    exit(2);
  }

  execvpe(argv[0], argv, envp); // hope it doesn't do an infinite loop
}

void
voosh_rebuild_urself(int argc, char **argv, char **envp) {
#ifdef VOOSH_FEATURE_ENV_VARS
  Raiz_CStrings env = {0};
  for (int i = 0; envp[i] != NULL; i++) {
    if (strneq(envp[i], "VOOSH", 5)) {
      raiz_da_append(&env, envp[i]);
    }
  }

  RAIZ_TODO("voosh: use `envp` for voosh itself");
#endif // VOOSH_FEATURE_ENV_VARS

  (void) argc; // silence unused variable warning

  // @todo maybe find a better way to get __FILE__ path
  if (needs_rebuild(argv[0], "./"__FILE__)) {
    rebuild(argv, envp);
  }
}
