#pragma once

#include "debug/logs.h"

#define RAIZ_ASSERT(expression, ...)\
  do\
  {\
    if (!(expression))\
    {\
      RAIZ_LOG(RAIZ_LOG_ERROR,\
          "assertion \"%s\" failed:\n", #expression);\
      fprintf(stderr, __VA_ARGS__);\
      abort();\
    }\
  } while (0)

