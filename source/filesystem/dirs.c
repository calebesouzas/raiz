#include "dirs.h"

/**
 * @doc!
 * @todo: finish implementing
 * @extra: i really don't know how to walk directories, and it's recursive, \
 * so it is even worse!
 * @func: raiz_dir_walk
 * @desc: Walks `path` directory running `func` for each object
 * @return: result where `false` means failure
 * @issue(safety): it may stack overflow (due to no recursion limit)!
 */
bool
raiz_dir_walk_recursive(
    const char *parent_path,
    Raiz_DirWalkFunc func,
    void *data
) {
  DIR *dir_p = opendir(parent_path);
  if (dir_p == NULL) return false;

  struct dirent *entry;
  while ((entry = readdir(dir_p)) != NULL) {

    char path_buffer[RAIZ_MAX_FILE_NAME] = {0};

    const char *child_path = entry->d_name;

    struct stat child_status;
    stat(child_path, &child_status);

    if (S_ISDIR(child_status.st_mode)) {
      size_t child_len = strlen(child_path);

      // skip `./` and `../` special folders
      if (strncmp(child_path, ".", child_len) == 0
          || strncmp(child_path, "..", child_len) == 0) continue;

      snprintf(path_buffer, sizeof(path_buffer),
          "%s/%s", parent_path, child_path);

      func(path_buffer, child_status.st_mode, data);

      if (!raiz_dir_walk_recursive(path_buffer, func, data)) goto fail;
    }

    func(child_path, child_status.st_mode, data);
  }

  closedir(dir_p);
  return true;
fail:
  closedir(dir_p);
  return false;
}
