#include "dirs.h"

static inline bool
is_special_path(const char *path) {
  size_t len = strlen(path);
  return strncmp(path, ".", len) == 0 || strncmp(path, "..", len) == 0;
}

bool
raiz_dir_walk_recursive(
    const char *parent_path,
    Raiz_DirWalkFunc func,
    void *data
)
{
  DIR *dir_p = opendir(parent_path);
  if (dir_p == NULL) return false;

  struct dirent *entry;
  while ((entry = readdir(dir_p)) != NULL)
  {
    char path_buffer[RAIZ_MAX_FILE_NAME] = {0};
    const size_t path_size = sizeof(path_buffer);

    const char *child_path = entry->d_name;

    snprintf(path_buffer, path_size, "%s/%s", parent_path, child_path);

    struct stat child_status;
    stat(path_buffer, &child_status);

    if (S_ISDIR(child_status.st_mode))
    {

      // skip `./` and `../` special folders
      if (is_special_path(child_path)) continue;

      func(path_buffer, child_status.st_mode, data);

      (void) raiz_dir_walk_recursive(path_buffer, func, data);
    }
    else
    {
      // @todo: handle other types of objects
      func(path_buffer, child_status.st_mode, data);
    }
  }

  closedir(dir_p);
  return true;
}

static void
get_relative_file_paths(const char *path, mode_t mode, void *raw_data)
{
  Raiz_CStrings *paths = raw_data;
  if (S_ISREG(mode)) raiz_da_append(paths, strdup(path));
}

Raiz_CStrings
raiz_dir_get_relative_file_paths(const char *dir_path)
{
  Raiz_CStrings paths = {0};
  raiz_dir_walk_recursive(dir_path, get_relative_file_paths, &paths);
  return paths;
}
