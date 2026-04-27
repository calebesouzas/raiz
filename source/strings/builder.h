#pragma once

typedef struct
{
  char *items;
  size_t count, capacity;
} Raiz_StringBuilder;

Raiz_StringBuilder raiz_strb_from_cstr(char *cstr);

Raiz_StringBuilder raiz_str_to_strb(Raiz_String *str);

void raiz_strb_append(Raiz_StringBuilder *sb, Raiz_String *s);

void raiz_strb_append_cstr(Raiz_StringBuilder *sb, char *cstr);
