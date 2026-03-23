#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define RAIZ_SOURCE "raiz.c"
#define OUTPUT_PATH BUILD_FOLDER"raiz"
#define CC_ARGS "-Wall", "-Wextra", "-ggdb"

bool build_raiz(void)
{
  Cmd cmd = {0};
  cmd_append(&cmd, "clang", "-o", OUTPUT_PATH, RAIZ_SOURCE, CC_ARGS);

  if (!cmd_run(&cmd)) return false;

  return true;
}

int main(int argc, char* argv[])
{
  GO_REBUILD_URSELF(argc, argv);

  if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

  if (!build_raiz()) return 2;

  if (argc > 1)
  {
    Cmd cmd = {0};

    if (strncmp(argv[1], "run", 3) == 0)
    {
      cmd_append(&cmd, BUILD_FOLDER"raiz");
    }
    else if (strncmp(argv[1], "debug", 5) == 0)
    {
      cmd_append(&cmd, "gdb", BUILD_FOLDER"raiz");
    }
    else if (strncmp(argv[1], "strict", 6) == 0)
    {
      // TODO: build with strict flags
    }

    if (!cmd_run(&cmd)) return 1;
  }
  return 0;
}
