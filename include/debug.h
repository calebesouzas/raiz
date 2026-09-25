#ifndef RAIZ_DEBUG_H
#define RAIZ_DEBUG_H

#define debug(...)\
do {\
  fprintf(stderr, "log@%s:%d:%s(): ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf(stderr, __VA_ARGS__);\
} while (0)

#endif /* RAIZ_DEBUG_H */
