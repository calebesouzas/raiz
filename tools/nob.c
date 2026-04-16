#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "../build/"
#define MAIN_BIN_PATH BUILD_FOLDER "raiz"

bool build(bool);

int
main(int argc, char **argv) {
  GO_REBUILD_URSELF(argc, argv);

  bool strict = false;

  for (int i = 0; i < argc; ++i) {
    if (strncmp(argv[i], "--strict", 8) == 0) {
      strict = true;
    }
  }

  return build(strict) ? 0 : 1;
}

bool
build(bool strict) {
  File_Paths paths = {0};
  if (!read_entire_dir("../source/", &paths)) {
    nob_log(NOB_ERROR, "failed to open `source/` folder");
    return false;
  }

  Cmd cmd = {0};

  cmd_append(&cmd, "clang", "-o", MAIN_BIN_PATH, "-Wall", "-Wextra");

  for (int i = 0; i < paths.count; ++i) {
    const char *path = paths.items[i];
    da_append(&cmd, path);
  }

  if (strict) {
    cmd_append(&cmd, "-Werror", "-pedantic", "-fsanatize=memory,undefined");
  }

  return cmd_run(&cmd);
}
