#pragma once

/**
 * @doc!
 * @name(header) common/strings.h
 * @tags helper
 * @desc string operations made easy
 */

/**
 * @doc!
 * @name(type) String
 * @tags helper
 * @todo decide which one to use between `ptr; len` and `data; size`
 */
typedef struct {
  char *data;
  size_t size;
} Raiz_String;

Raiz_String raiz_str_from(char *cstr);
