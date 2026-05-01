#pragma once

#include <sys/wait.h> // waitpid()

#include "common/libc.h"
#include "debug/logs.h" // RAIZ_LOG
#include "filesystem/file.h" // file_read()
#include "filesystem/time.h" // get_<...>_time()
#include "strings/builder.h"

/*@doc! usage
Call it and the Raiz project will rebuild itself if needed.
*/
void raiz_rebuild_urself(char **argv);
