#ifdef _WIN32
#error "Raiz is not implemented on Windows"
#endif

#include "common/libc.h"
#include "errors/panics.h"
#include "dev/rebuild.h"

int
main(int argc, char **argv)
{
  raiz_rebuild_urself(argv);

  if (argc > 1)
  {
    RAIZ_TODO("open file");
  }
  else
  {
    RAIZ_TODO("run REPL");
  }

  return 0;
}

// implementations:
#include "sources.c"
