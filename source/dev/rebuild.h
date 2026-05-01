#pragma once

#include "common/libc.h"
#include "debug/logs.h"
#include "filesystem/file.h"
#include "filesystem/time.h"
#include "shell/commands.h"
#include "strings/builder.h"

/*@doc! usage
Call it and the Raiz project will rebuild itself if needed.
*/
void raiz_rebuild_urself(int argc, char **argv);
