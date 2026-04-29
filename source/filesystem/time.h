#pragma once

#include <sys/stat.h>

int raiz_file_get_last_mod_time(const char *const path);
int raiz_file_get_creation_time(const char *const path);
int raiz_file_get_last_access_time(const char *const path);
