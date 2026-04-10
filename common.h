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

Rz_String rz_string_cstr(char *c_string);
Rz_String rz_string_cstr_slice(char *c_string, size_t len);
Rz_String rz_string_slice(Rz_String *other, size_t start, size_t end);
Rz_String rz_string_clone(Rz_String *other);
void rz_string_free(Rz_String *string);

#endif // RAIZ_COMMON_HEADERS_H
