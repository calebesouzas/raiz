#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"
#define CC_ARGS "-Wall", "-Wextra", "-ggdb"

int main(int argc, char* argv[]) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "clang", "-o", BUILD_FOLDER"raiz", "raiz.c", CC_ARGS);

  if (!nob_cmd_run(&cmd)) return 1;

  if (argc > 1 && strncmp(argv[1], "run", 3) == 0) {
    nob_cmd_append(&cmd, "./build/raiz");
    if (!nob_cmd_run(&cmd)) return 1;
  }
  return 0;
}
