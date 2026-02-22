#ifndef RAIZ_ERRORS_H
#define RAIZ_ERRORS_H

#define RAIZ_PRINT_ERR(...)\
  fprintf(stderr, ##__VA_ARGS__)

#define RAIZ_PANIC(...)\
  fprintf(stderr, ##__VA_ARGS__);\
  exit(1)

#endif /* ifndef RAIZ_ERRORS_H */
