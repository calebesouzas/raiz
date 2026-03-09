#ifndef RAIZ_STRINGS_H
#define RAIZ_STRINGS_H

#include <assert.h>
#define RAIZ_STRING_CAPACITY (256)

#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int len;
  unsigned int capacity;
} StringHeader;

// string_push: pushes 'source' into 'destine', does realloc the header if
// needed or inits 'destine' if it's NULL. Returns non-zero values for errors
#define string_push(destine, source)\
  string_push_slice(destine, source, strlen(source))

#define string_push_slice(destine, source, source_len)\
do {\
  unsigned int destine_len = (destine) ? strlen(destine) : 0;\
\
  StringHeader* header = NULL;\
\
  if ((destine) == NULL) {\
    header = malloc(RAIZ_STRING_CAPACITY + sizeof(StringHeader));\
\
    if (header != NULL) {\
      header->capacity = RAIZ_STRING_CAPACITY;\
      header->len = 0;\
\
      (destine) = (char*)(header + 1);\
    }\
  }\
  header = (StringHeader*)(destine) - 1;\
  while (header->capacity <= (source_len + destine_len)) {\
    header->capacity *= 1.5;\
    header = realloc(header, header->capacity + sizeof(StringHeader));\
  }\
\
  if (header != NULL) {\
    strncat((destine), (source), (source_len));\
    header->len += source_len;\
  }\
\
} while (0)

#define string_free(string) free((StringHeader*)(string) - 1)
#endif // RAIZ_STRINGS_H
