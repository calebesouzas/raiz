#ifndef RAIZ_STRINGS_C
#define RAIZ_STRINGS_C

size_t salloc(size_t amount) {
  char *s = malloc(amount);
  return sput(s);
}

size_t sput(char *cstr) {
  da_add(&String_Pool, cstr);
  return String_Pool.len - 1;
}

size_t ssave(char *source) {
  size_t len = strlen(source);
  return snsave(source, len);
}

size_t snsave(char *source, size_t size) {
  size_t index = salloc(size + 1);
  char *s = sget(index);

  strncpy(s, source, size);
  s[size] = '\0';

  return index;
}

void sfree(size_t index) {
  free(String_Pool.dat[index]);
  String_Pool.dat[index] = NULL;
}

char *sget(size_t index) {
  return String_Pool.dat[index];
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
