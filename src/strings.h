#ifndef RAIZ_STRINGS_H
#define RAIZ_STRINGS_H

#define RAIZ_STRING_CAPACITY (256)

#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int len;
  unsigned int capacity;
} StringHeader;

// string_push: pushes 'source' into 'destine', does realloc the header if
// needed or inits 'destine' if it's NULL. Returns non-zero values for errors
int string_push(char* destine, char*const source) {
  unsigned int source_len = strlen(source);
  unsigned int destine_len = strlen(source);

  if (destine == NULL) {
    StringHeader *header = malloc(RAIZ_STRING_CAPACITY + sizeof(StringHeader));
    if (!header) return 1;
  }
  StringHeader *header = (StringHeader *)(destine - 1);

  while (header->capacity <= (source_len + destine_len)) {
    header->capacity *= 1.5;
    header = realloc(header, header->capacity + sizeof(StringHeader));
    if (!header) { return 2; }
  }

  memcpy(header + 1, source, source_len); // hope it doesn't seg fault...

  return 0;
}

#define string_free(string) free((StringHeader*)(string - 1))
#endif // RAIZ_STRINGS_H
