#include "libc.h"
#include "files.h"

bool process_program(const char *source, size_t size);

int main(int argc, char **argv)
{
  if (argc <= 1)
  {
    fprintf(stderr, "usage: %s <raiz file>", argv[0]);
    return 1;
  }

  char *source = NULL;
  size_t size = 0;
  if (!read_entire_file(argv[1], &source, &size))
    return 1;

  if (!process_program(source, size))
    return 1;

  free(source);
  source = NULL;

  return 0;
}

bool process_program(const char *source, const size_t size)
{
  (void) source;
  (void) size;
  return true;
}
