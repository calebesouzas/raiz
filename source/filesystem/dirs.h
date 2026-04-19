#pragma once

#include "common/libc.h"
#include <dirent.h>
#include <sys/stat.h>

#define RAIZ_MAX_FILE_NAME 1024

typedef void (*Raiz_DirWalkFunc)(const char *path, mode_t mode, void *data);

bool raiz_dir_walk_recursive(
    const char *path, Raiz_DirWalkFunc func, void *data);
