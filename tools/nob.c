#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#include "filesystem/dirs.h"

#define BUILD_FOLDER "build/"
#define MAIN_BIN_PATH BUILD_FOLDER "raiz"

void
rebuild_urself(void) {
  Cmd cmd = {0};

  cmd_append(&cmd, "clang", "-I./source", "-o", "./tools/nob");
  cmd_append(&cmd, "./tools/nob.c", "./source/filesystem/dirs.c");

  if (!cmd_run(&cmd)) exit(-1);
}

bool build(bool);

int
main(int argc, char **argv) {
  rebuild_urself();

  bool strict = false;

  for (int i = 0; i < argc; ++i) {
    if (strncmp(argv[i], "--strict", 8) == 0) {
      strict = true;
    }
  }

  return build(strict) ? 0 : 1;
}

void
get_file_paths_in_source(const char *path, mode_t mode, void *raw_data) {
  File_Paths *paths = raw_data;

  if (S_ISREG(mode)) {
    // Raiz has literally the same implementation of dynamic arrays as Nob
    if (path[strlen(path) - 1] == 'c') {
      nob_da_append(paths, strdup(path));
    }
  }
}

void
free_paths(File_Paths *paths) {
  for (size_t i = 0; i < paths->count; ++i) {
    free(paths->items[i]);
  }
}

bool
build(bool strict) {
  File_Paths paths = {0};
  if (!raiz_dir_walk_recursive("./source", get_file_paths_in_source, &paths)) {
    nob_log(NOB_ERROR, "Couldn't read directory `source/`\n");
    free_paths(&paths);
    return false;
  }

  Cmd cmd = {0};
  cmd_append(&cmd, "clang", "-o", MAIN_BIN_PATH);

  for (int i = 0; i < paths.count; ++i) {
    char *path = paths.items[i];
    cmd_append(&cmd, path);
  }

  cmd_append(&cmd, "-I./source/", "-Wall", "-Wextra");

  if (strict) {
    cmd_append(&cmd, "-Werror", "-pedantic", "-fsanatize=memory,undefined");
  }

  if (!cmd_run(&cmd)) {
    free_paths(&paths);
    return false;
  }

  free_paths(&paths);
  return true;
}
