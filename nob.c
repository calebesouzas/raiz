#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define RAIZ_SOURCE "raiz.c"
#define OUTPUT_PATH BUILD_FOLDER"raiz"
#define CC_ARGS "-Wall", "-Wextra", "-ggdb"

bool build_raiz(bool no_logs, bool strict)
{
  Cmd cmd = {0};
  cmd_append(&cmd, "clang", "-o", OUTPUT_PATH, CC_ARGS);

  const char *sources[] = {
    "common.c",
    "expressions.c",
    "lexer.c",
    "maps.c",
    "parser.c",
    "raiz.c",
    "runtime.c",
  };

  for (int i = 0; i < ARRAY_LEN(sources); ++i)
  {
    cmd_append(&cmd, sources[i]);
  }

  if (no_logs)
  {
    cmd_append(&cmd, "-DRZ_DISABLE_LOGS");
  }
  if (strict)
  {
    cmd_append(&cmd, "-Werror", "-pedantic", "-Wpedantic");
  }

  if (!cmd_run(&cmd)) return false;

  return true;
}

int main(int argc, char* argv[])
{
  GO_REBUILD_URSELF(argc, argv);

  if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

  bool no_logs = false;
  bool strict = false;
  bool run_after = false;
  bool debug = false;

  if (argc > 1)
  {
    for (int i = 1; i < argc; i++)
    {
      if (strncmp(argv[i], "run", 3) == 0)
      {
        run_after = true;
      }
      else if (strncmp(argv[i], "debug", 5) == 0)
      {
        debug = true;
      }
      else if (strncmp(argv[i], "--strict", 8) == 0)
      {
        strict = true;
      }
      else if (strncmp(argv[i], "--no-logs", 9) == 0)
      {
        no_logs = true;
      }
    }
  }
  if (!build_raiz(no_logs, strict)) return 2;
  if (run_after)
  {
    Cmd cmd = {0};
    cmd_append(&cmd, OUTPUT_PATH);
    if (!cmd_run(&cmd)) return 3;
  }
  else if (debug)
  {
    Cmd cmd = {0};
    cmd_append(&cmd, "gdb", OUTPUT_PATH);
    if (!cmd_run(&cmd)) return 3;
  }
  return 0;
}
