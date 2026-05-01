#pragma once

typedef struct
{
  char *items;
  size_t count;
} Raiz_String;

Raiz_String raiz_str_from_cstr(char *cstr);
