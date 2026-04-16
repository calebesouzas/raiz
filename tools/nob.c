#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

int
main(int argc, char **argv) {
  GO_REBUILD_URSELF(argc, argv);
  printf("Hello from Nob!\n");
  return 0;
}
