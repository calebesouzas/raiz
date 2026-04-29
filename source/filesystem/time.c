#pragma once

#include "time.h"

static struct stat
get_status(const char *const path)
{
  struct stat status;
  stat(path, &status);
  return status;
}

int
raiz_file_get_last_mod_time(const char *const path)
{
  return get_status(path).st_mtim.tv_sec;
}

int
raiz_file_get_creation_time(const char *const path)
{
  return get_status(path).st_ctim.tv_sec;
}

int
raiz_file_get_last_access_time(const char *const path)
{
  return get_status(path).st_atim.tv_sec;
}
