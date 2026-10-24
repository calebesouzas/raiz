#pragma once

#include "libc.h"

#define assert_allocation(pointer)\
do\
{\
  if ((pointer) == NULL)\
  {\
    fprintf(stderr, "%s:%d: `%s` allocation failed\n", __FILE__, __LINE__, #pointer);\
    abort();\
  }\
} while (0)

#define just_assert(condition)\
do\
{\
  if (!(condition))\
  {\
    fprintf(stderr, "%s:%d: `%s` condition failed\n", __FILE__, __LINE__, #condition);\
    abort();\
  }\
} while (0)
