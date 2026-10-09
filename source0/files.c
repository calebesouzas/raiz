#include "files.h"

bool read_entire_file(const char *file_path, char **buffer, size_t *size)
{
  bool result = true;

  FILE *file = fopen(file_path, "r");
  if (file == NULL)
  {
    fprintf(stderr, "%s(): failed to open '%s': %s\n", __FUNCTION__, file_path, strerror(errno));
    return false;
  }

  fseek(file, 0, SEEK_END);
  *size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *buffer = malloc(*size);
  if (*buffer == NULL)
  {
    fprintf(stderr, "%s(): failed to allocate %zu bytes\n", __FUNCTION__, *size);
    result = false;
    goto close_file;
  }

  size_t bytes_read = fread(*buffer, sizeof(char), *size, file);
  if (bytes_read != *size)
  {
    fprintf(
        stderr, "%s(): failed to read %zu bytes, could read only %zu\n",
        __FUNCTION__, *size, bytes_read
    );
    result = false;
  }

close_file:
  fclose(file);

  return result;
}
