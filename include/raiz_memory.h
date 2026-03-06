#ifndef RAIZ_MEMORY_H
#define RAIZ_MEMORY_H

#define KB (1024)
#define MB (KB * 1024)
#define GB (MB * 1024)

typedef unsigned char byte;

#define raiz_malloc(size, reason)\
  fprintf(stderr, "[MALLOC] For \"%s\" at %s line %d", reason, __FILE__, __LINE__)

#endif /* RAIZ_MEMORY_H */
