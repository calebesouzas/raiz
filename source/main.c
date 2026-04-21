#include "common/libc.h"
#include "errors/panics.h"

int
main(int argc, char **argv) {
  (void) argv;

  if (argc > 1) {
    RAIZ_TODO("open file");
  }

  return 0;
}
