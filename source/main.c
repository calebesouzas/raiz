#include "common/libc.h"
#include "errors/panics.h"

int
main(int argc, char **argv) {
  (void) argv;

  if (argc > 1) {
    RAIZ_TODO("open file");
  } else {
    RAIZ_TODO("run REPL");
  }

  return 0;
}
