#ifndef RAIZ_COMMON_HEADERS_H
#define RAIZ_COMMON_HEADERS_H

// C standard lib headers
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// Raiz headers
#include "raiz.h"
#include "debug.h"

// Some essencial structs
typedef struct
{
  char *data;
  size_t size;
} Rz_String;

#ifndef RZ_SV
#define RZ_SV(sv) (sv).size, (sv).data
#endif // RZ_SV

#endif // RAIZ_COMMON_HEADERS_H
