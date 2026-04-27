#pragma once

#define RAIZ_PANIC(...)\
  do\
  {\
    const char *file = __FILE__;\
    const char *func = __func__;\
    const int line = __LINE__;\
    fprintf(stderr, "raiz: panicked at %s:%d:%s():\n", file, line, func);\
    fprintf(stderr, __VA_ARGS__);\
    abort();\
  } while (0)

#define RAIZ_TODO(message) RAIZ_PANIC("TODO: %s\n", (message))
