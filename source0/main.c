#include "libc.h"

bool read_entire_file(const char *file_path, char **p_buffer, size_t *p_size)
{
  bool result = true;

  FILE *file = fopen(file_path, "r");
  if (file == NULL)
  {
    fprintf(stderr, "%s(): failed to open '%s': %s\n", __FUNCTION__, file_path, strerror(errno));
    return false;
  }

  fseek(file, 0, SEEK_END);
  *p_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *p_buffer = malloc(*p_size);
  if (*p_buffer == NULL)
  {
    fprintf(stderr, "%s(): failed to allocate %zu bytes\n", __FUNCTION__, *p_size);
    result = false;
    goto close_file;
  }

  size_t bytes_read = fread(*p_buffer, sizeof(char), *p_size, file);
  if (bytes_read != *p_size)
  {
    fprintf(
        stderr, "%s(): failed to read %zu bytes, could read only %zu\n",
        __FUNCTION__, *p_size, bytes_read
    );
    result = false;
  }

close_file:
  fclose(file);

  return result;
}

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
