#include "errors.h"

// better information at the header
// i'm not sure if using a macro instead is a better choice
RaizError raiz_new_error(
  char *const kind,
  char *const message,
  RaizErrorLevel level
) {
  return (RaizError){
    .kind = kind,
    .message = message,
    .level = level
  };
}
