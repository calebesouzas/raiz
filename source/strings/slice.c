#include "slice.h"

Raiz_String
raiz_str_from_cstr(char *cstr)
{
  return (Raiz_String) {.data = cstr, .size = strlen(cstr)};
}
