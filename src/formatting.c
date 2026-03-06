#include "formatting.h"
#include "raiz_errors.h"
#include "raiz_strings.h"
#include <assert.h>
#include <malloc.h>
#include <stdarg.h>

RaizResult raiz_format(int count, ...) {
  va_list args;
  va_start(args, count);

  char* result = NULL;

  for (int i = 0; i < count; ++i) {
    result = string_push(result, va_arg(args, char*));

    if (result == NULL) return (RaizResult) {
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
