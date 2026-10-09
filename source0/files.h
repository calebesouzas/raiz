#pragma once

#include "libc.h"

bool read_entire_file(const char *file_path, char **buffer, size_t *size);
