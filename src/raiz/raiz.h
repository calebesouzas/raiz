#ifndef RAIZ_H
#define RAIZ_H

#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "memory.h"
#include "arrays.h"
#include "strings.h"

#ifndef RAIZ_FILE_SIZE_LIMIT
#define RAIZ_FILE_SIZE_LIMIT (8 * MB)
#endif

#ifndef RAIZ_VERSION
#include "version.def"
#endif

#ifdef RAIZ_IMPLEMENTATION
#include "lexer.c"
#endif // RAIZ_IMPLEMENTATION

#endif // RAIZ_H
