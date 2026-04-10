#ifndef RAIZ_COMMON_SOURCE
#define RAIZ_COMMON_SOURCE

#include "common.h"

Rz_String rz_string_cstr(char *c_string)
{
  return (Rz_String) {.data = c_string, .size = strlen(c_string) };
}

Rz_String rz_string_cstr_slice(char *c_string, size_t len)
{
  return (Rz_String) {.data = c_string, .size = len };
}

Rz_String rz_string_slice(Rz_String *other, size_t start, size_t end)
{
  return (Rz_String) {.data = strdup(other->data + start), .size = start - end};
}

Rz_String rz_string_clone(Rz_String *other)
{
  return (Rz_String) {.data = strdup(other->data), .size = other->size };
}

void rz_string_free(Rz_String *string)
{
  free(string->data);
  string->data = NULL;
  string->size = 0;
}

#endif // RAIZ_COMMON_SOURCE
