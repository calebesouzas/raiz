#pragma once

/**
 * @doc!
 * @name(header) strings/array.h
 * @tags helper, dynamic, arrays
 * @desc string operations made easy
 */

/**
 * @doc!
 * @name(type) Strings
 * @desc [@da] of [@String]s
 */
typedef struct {
  Raiz_String *items;
  size_t count, capacity;
} Raiz_Strings;

/**
 * @doc!
 * @name(type) CStrings
 * @desc [@da] of C style strings
 */
typedef struct {
  char **items;
  size_t count, capacity;
} Raiz_CStrings;
