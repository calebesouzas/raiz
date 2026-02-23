#include "rz_strings.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

String String_new(char *const s) {
  // uint32_t len;
  // GET_STRING_LEN(len, s);

  return (String) {
    .ptr = s,
    .len = strlen(s)
  };
}

DynamicString* DynamicString_new(void) {
  DynamicString* res = malloc(sizeof(DynamicString));
  
  if (res != NULL) {
    res->capacity = RZ_DEFAULT_STRING_CAPACITY;

    res->ptr = (char *)malloc(res->capacity);
    if (res->ptr == NULL) { return NULL; }

    res->len = 0;
  }

  return res;
}

DynamicString* DynamicString_from(String s) {
  DynamicString* res = malloc(sizeof(DynamicString));

  if (res != NULL) {
    res -> capacity = RZ_DEFAULT_STRING_CAPACITY;
    if (s.len > res->capacity) {
      res->capacity *= 2;
    }

    res->len = s.len;

    res->ptr = malloc(res->capacity);
    if (res->ptr == NULL) { return NULL; }
  
    strncpy(res->ptr, s.ptr, res->len);
  }

  return res;
}

int DynamicString_push(DynamicString *destination, String source) {
  uint32_t i = 0;
  
  if (destination->capacity < source.len) {
    destination->capacity *= 2;
    destination->ptr = realloc(
      destination->ptr, destination->capacity
    );
    if (destination->ptr == NULL) { return -1; }
  }

  while (i < source.len) {
    destination->ptr[destination->len] = source.ptr[i];
    destination->len++;

    i++;
  }

  return 0;
}

int String_eq(String *left, String *right) {
  int result;

  for (uint32_t i = 0; i < left->len; ++i) {
    if (left->len == right->len) {
      result = 1;
      continue;
    }
    else {
      result = 0;
      break;
    }
  }

  return result;
}
