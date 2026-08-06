#ifndef RAIZ_STRINGS_H
#define RAIZ_STRINGS_H

typedef struct {
  char **dat;
  size_t len, cap;
} StringPool;

StringPool String_Pool = {0};

#define TEMP_BUF_CAP 1024
char Temp_Buffer[TEMP_BUF_CAP] = {0};

size_t salloc(char *cstr);

char *temp_vsnprintf(size_t n, const char *f, va_list v)
  __attribute__((format(printf, 2, 0)));

char *temp_snprintf(size_t n, const char *f, ...)
  __attribute__((format(printf, 2, 3)));

char *temp_vsprintf(const char *f, va_list v)
  __attribute__((format(printf, 1, 0)));

char *temp_sprintf(const char *f, ...)
  __attribute__((format(printf, 1, 2)));

#endif // RAIZ_STRINGS_H
