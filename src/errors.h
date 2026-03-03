#ifndef RAIZ_ERRORS_H
#define RAIZ_ERRORS_H

typedef enum {
  RAIZ_LEVEL_HINT,
  RAIZ_LEVEL_NOTE,
  RAIZ_LEVEL_WARNING,
  RAIZ_LEVEL_ERROR,
} RaizErrorLevel;

typedef struct {
  RaizErrorLevel level;
  char* kind;
  char* message;
} RaizError;

typedef struct {
  enum {
    RAIZ_RESULT_OK,
    RAIZ_RESULT_ERROR,
  } status;
  void *ok_value;
  RaizError err_value;
} RaizResult;

RaizError raiz_new_error(
  char*const kind, // e.g. "lexical", "syntax", "semantic"...
  char*const message, // no support for formatted messages yet
  RaizErrorLevel level
);

#ifdef RAIZ_ERRORS_IMPLEMENTATION
#include "errors.c"
#endif

#endif /* ifndef RAIZ_ERRORS_H */
