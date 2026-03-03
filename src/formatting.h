#ifndef RAIZ_FORMATTING_H
#define RAIZ_FORMATTING_H

#include "errors.h"
RaizResult raiz_format(int count, ...);

#ifdef RAIZ_FORMATTING_IMPLEMENTATION
#include "formatting.c"
#endif

#endif
