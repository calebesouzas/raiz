#ifndef RAIZ_RZ_STRINGS_H
#define RAIZ_RZ_STRINGS_H

#include <stdint.h>
typedef struct {
  char *ptr;
  uint32_t len;
} String;

#define GET_STRING_LEN(len, string)\
  for (len = 0; string[len] != '\0'; ++len)


#endif /* ifndef RAIZ_RZ_STRINGS_H */
