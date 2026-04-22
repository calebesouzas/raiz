#include "dirs.h"

/**
 * @doc!
 * @name(function) is_special_path
 * @tags helper, inline
 * @param `path` file/folder name
 * @return(bool) `true` if `path` == "." or `path` == ".."
 */
static inline bool
is_special_path(const char *path) {
  size_t len = strlen(path);
  return strncmp(path, ".", len) == 0 || strncmp(path, "..", len) == 0;
}

/**
 * @doc!
 * @name(function) raiz_dir_walk
 * @desc Walks `path` directory running `func` for each object
 * @return(bool) `false` means failure
 * @issue(safety) it may stack overflow (due to no recursion limit)!
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
    const size_t path_size = sizeof(path_buffer);

    const char *child_path = entry->d_name;

    snprintf(path_buffer, path_size, "%s/%s", parent_path, child_path);

    struct stat child_status;
    stat(path_buffer, &child_status);

    if (S_ISDIR(child_status.st_mode)) {

      // skip `./` and `../` special folders
      if (is_special_path(child_path)) continue;

      func(path_buffer, child_status.st_mode, data);

      (void) raiz_dir_walk_recursive(path_buffer, func, data);
    } else {
      // @todo: handle other types of objects
      func(path_buffer, child_status.st_mode, data);
    }
  }

  closedir(dir_p);
  return true;
}
