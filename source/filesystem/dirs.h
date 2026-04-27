#pragma once

#include <dirent.h>
#include <sys/stat.h>

#define RAIZ_MAX_FILE_NAME 1024

typedef void (*Raiz_DirWalkFunc)(const char *path, mode_t mode, void *data);

bool raiz_dir_walk_recursive(
    const char *path, Raiz_DirWalkFunc func, void *data);

Raiz_CStrings raiz_dir_get_relative_file_paths(const char *dir_path);
