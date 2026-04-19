#include "filesystem/dirs.h"

bool
print_object_type(const char *path, mode_t mode, void *data) {
  int level = *(int*)data; // dereference data as int pointer

  if (path[0] == '.') {
    return true; // just ignore them for now
  }

  for (int i = 0; i < level; ++i) printf(" ");

  if (S_ISREG(mode)) {
    printf("\\ %s\n", path);
  } else if (S_ISDIR(mode)) {
    printf("\\ %s/\n", path);
    level++;
  } else {
    printf("\n>> %s is an unhandled or invalid object type\n", path);
  }

  return true;
}

int
main(void) {
  int level = 0;
  return raiz_dir_walk_recursive(".", print_object_type, &level) ? 0 : 1;
}
