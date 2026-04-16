#pragma once

#define RAIZ_PANIC(...)\
  do {\
    fprintf("raiz: panicked at %s:%d:%s():\n", __FILE__,__LINE__,__func__);\
    fprintf(stderr, __VA_ARGS__);\
    abort();\
  } while (0)

#define RAIZ_TODO(message) RAIZ_PANIC("TODO: %s\n", (message))
