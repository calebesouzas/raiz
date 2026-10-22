#include <stdarg.h>

#include "report.h"

void report(Span *span, const char *format, ...)
{
  fprintf(stderr, "%s:%zu:%zu:[%zu]: ", span->file_path, span->line, span->column, span->index);

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}
