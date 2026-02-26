#ifndef RAIZ_RZ_STRINGS_H
#define RAIZ_RZ_STRINGS_H

#include "memory.h"

#include <stdint.h>
typedef struct {
  char *ptr;
  uint32_t len;
} String;

typedef struct {
  char *ptr;
  uint32_t len;
  uint32_t capacity;
} DynamicString;

#define GET_STRING_LEN(len, string)\
  for (len = 0; string[len] != '\0'; ++len)

#define RZ_DEFAULT_STRING_CAPACITY (8 * KB)

String String_new(char *const s);

DynamicString* DynamicString_new(void);
DynamicString* DynamicString_from(String s);

int DynamicString_push(DynamicString* destination, String source);

int String_eq(String *left, String *right);

#endif /* ifndef RAIZ_RZ_STRINGS_H */
