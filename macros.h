#ifndef RAIZ_MACROS_H
#define RAIZ_MACROS_H

#define PANIC(...)\
  do {\
    fprintf(stderr, "internal panic at %s:%s:%zu\n",\
            __FILE__, __func__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    exit(-1);\
  } while (0)

#define TODO(...)\
  do {\
    fprintf(stderr, "internal TODO at %s:%s:%zu\n",\
            __FILE__, __func__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    exit(-1);\
  } while (0)

#define UNREACHABLE(...)\
  do {\
    fprintf(stderr, "reached internal unreachable code at %s:%s:%zu\n",\
            __FILE__, __func__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    exit(-1);\
  } while (0)

#endif // RAIZ_MACROS_H
