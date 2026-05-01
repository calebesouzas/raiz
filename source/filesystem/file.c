#include "file.h"

Raiz_StringBuilder
raiz_file_read(char *path)
{
  FILE *file_p = fopen(path, "r");
  if (file_p == NULL)
  {
    RAIZ_LOG(RAIZ_LOG_ERROR,
        "failed to open %s: %s", path, strerror(errno));
    return (Raiz_StringBuilder){0};
  }

  Raiz_StringBuilder result = {0};

  char line_buffer[1024] = {0};
  while (fgets(line_buffer, sizeof(line_buffer), file_p) != NULL)
  {
    raiz_strb_append_cstr(&result, line_buffer);
  }

  fclose(file_p);
  return result;
}
