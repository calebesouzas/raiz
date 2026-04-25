#pragma once

/**
 * @doc!
 * @name(type) StringBuilder
 * @desc a dynamic string
 * @note is not the same thing as [@Strings]
 */
typedef struct {
  char *items;
  size_t count, capacity;
} Raiz_StringBuilder;

Raiz_StringBuilder raiz_strb_from(char *cstr);
Raiz_StringBuilder raiz_str_to_strb(Raiz_String str);
