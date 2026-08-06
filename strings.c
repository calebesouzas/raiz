#ifndef RAIZ_STRINGS_C
#define RAIZ_STRINGS_C

size_t salloc(char *cstr) {
  da_add(&String_Pool, cstr);
  return String_Pool.len - 1;
}

char *temp_vsnprintf(size_t n, const char *f, va_list v) {
  vsnprintf(Temp_Buffer, n, f, v);
  return Temp_Buffer;
}

char *temp_snprintf(size_t n, const char *f, ...) {
  va_list a;
  va_start(a, f);
  char *s = temp_vsnprintf(n, f, a);
  va_end(a);
  return s;
}

char *temp_vsprintf(const char *f, va_list v) {
  vsprintf(Temp_Buffer, f, v);
  return Temp_Buffer;
}

char *temp_sprintf(const char *f, ...) {
  va_list a;
  va_start(a, f);
  char *s = temp_vsprintf(f, a);
  va_end(a);
  return s;
}

#endif // RAIZ_STRINGS_C
