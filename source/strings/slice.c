#include "slice.h"

Raiz_String
raiz_str_from_cstr(char *cstr)
{
  return (Raiz_String) {.items = cstr, .count = strlen(cstr)};
}
