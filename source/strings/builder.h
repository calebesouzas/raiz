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

/**
 * @doc!
 * @name(function) strb_from_cstr
 * @param `char *cstr` null terminated byte sequence
 * @note `cstr` is not validated in any way
 * @desc creates [@StringBuilder] from C style string
 */
Raiz_StringBuilder raiz_strb_from_cstr(char *cstr);

/**
 * @doc!
 * @name(function) str_to_strb
 * @param [@String] `*str` immutable reference to `str`
 * @desc creates [@StringBuilder] from [@String]
 */
Raiz_StringBuilder raiz_str_to_strb(Raiz_String *str);

/**
 * @doc!
 * @name(function) strb_append
 * @param `sb` pointer to [@StringBuilder]
 * @param `s` immutable reference to [@String]
 * @desc pushes [@String]'s data into a [@StringBuilder] pointed to by `sb`
 */
void raiz_strb_append(Raiz_StringBuilder *sb, Raiz_String *s);

/**
 * @doc!
 * @name(function) strb_append_cstr
 * @desc same as [@strb_append] but with a C style string
 */
void raiz_strb_append_cstr(Raiz_StringBuilder *sb, char *cstr);
