#include "formatting.h"
#include "errors.h"
#include "strings.h"
#include <stdarg.h>
#include <stdio.h>

RaizResult raiz_format(int count, ...) {
  va_list args;
  va_start(args, count);

  char* result = NULL;

  for (int i = 0; i < count; ++i) {
    int error = string_push(result, va_arg(args, char*));

    if (error) return (RaizResult) {
      .status = RAIZ_RESULT_ERROR,
      .ok_value = NULL,
      .err_value = (RaizError){
        .kind = "memory",
        .level = RAIZ_LEVEL_WARNING,
        .message = "failed to push string into result"
      }
    };
  }

  va_end(args);
  return (RaizResult) {
    .status = RAIZ_RESULT_OK,
    .ok_value = result,
  };
}
