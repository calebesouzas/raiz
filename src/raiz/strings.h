#ifndef RAIZ_STRINGS_H
#define RAIZ_STRINGS_H

#ifndef RAIZ_STRING_CAPACITY
#define RAIZ_STRING_CAPACITY (256)
#endif

typedef struct {
  size_t len;
  size_t capacity;
} Raiz_StringHeader;

// string_push: pushes 'source' into 'destine', does realloc the header if
// needed or inits 'destine' if it's NULL. Returns non-zero values for errors
#define raiz_string_push(destine, source)\
  raiz_string_push_slice(destine, source, strlen(source))

#define raiz_string_push_slice(destine, source, source_len)\
do {\
  size_t destine_len = (destine) ? strlen(destine) : 0;\
\
  Raiz_StringHeader* header = NULL;\
\
  if ((destine) == NULL) {\
    header = malloc(RAIZ_STRING_CAPACITY + sizeof(Raiz_StringHeader));\
\
    if (header != NULL) {\
      header->capacity = RAIZ_STRING_CAPACITY;\
      header->len = 0;\
\
      (destine) = (char*)(header + 1);\
    }\
  }\
  header = (Raiz_StringHeader*)(destine) - 1;\
  while (header->capacity <= (source_len + destine_len)) {\
    header->capacity *= 1.5;\
    header = realloc(header, header->capacity + sizeof(Raiz_StringHeader));\
  }\
\
  if (header != NULL) {\
    strncat((destine), (source), (source_len));\
    header->len += source_len;\
  }\
\
} while (0)

#define raiz_string_free(string) free((Raiz_StringHeader*)(string) - 1)
#endif // RAIZ_STRINGS_H
