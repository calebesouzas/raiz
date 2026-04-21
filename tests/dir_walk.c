#include "common/arrays.h"
#include "errors/panics.h"
#include "filesystem/dirs.h"

typedef struct {
  char **items;
  size_t count, capacity;
} FilePaths;

void
get_file_paths_in_source(const char *path, mode_t mode, void *raw_data) {
  if (strncmp(path, "./source", 8) != 0) return;

  FilePaths *paths = raw_data;

  if (S_ISREG(mode)) {
    raiz_da_append(paths, strdup(path));
  }
}

int
main(void) {
  FilePaths paths = {0};
  if (!raiz_dir_walk_recursive(".", get_file_paths_in_source, &paths)) return 1;

  for (size_t i = 0; i < paths.count; ++i) {
    char *file_path = paths.items[i];
    printf("%s\n", file_path);

    free(file_path);
    file_path = NULL;
  }

  return 0;
}
