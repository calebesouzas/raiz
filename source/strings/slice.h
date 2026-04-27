#pragma once

typedef struct
{
  char *data;
  size_t size;
} Raiz_String;

Raiz_String raiz_str_from_cstr(char *cstr);
