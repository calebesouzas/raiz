#ifndef RAIZ_STRINGS_C
#define RAIZ_STRINGS_C

// -- // -- // String Pool // -- // -- //
size_t sp_alloc(size_t amount) {
  char *s = malloc(amount);
  sb_t sb = {.ptr = s, .len = 0, .cap = amount};
  da_add(&String_Pool, sb);
  return String_Pool.len - 1;
}

size_t sp_put(sb_t sb) {
  da_add(&String_Pool, sb);
  return String_Pool.len - 1;
}

size_t sp_put_cstr(char *cstr) {
  da_add(&String_Pool, sb_from_cstr(cstr));
  return String_Pool.len - 1;
}

size_t sp_save(char *source) {
  size_t len = strlen(source);
  return sp_nsave(source, len);
}

size_t sp_nsave(char *source, size_t size) {
  size_t index = sp_alloc(size + 1);
  sb_t *sb = sp_get(index);

  strncpy(sb->ptr, source, size);
  sb->ptr[size] = '\0';
  sb->len = size;

  return index;
}

void sp_free(size_t index) {
  free(String_Pool.dat[index].ptr);
  String_Pool.dat[index] = (sb_t){0};
}

sb_t *sp_get(size_t index) {
  return &String_Pool.dat[index];
}

// -- // -- // String View & Builder // -- // -- //
sv_t sv_make(char *cstr) {
  return (sv_t){.ptr = cstr, .len = strlen(cstr)};
}

sb_t sb_make(sv_t sv) {
  return (sb_t){.ptr = strndup(sv.ptr, sv.len), .len = sv.len, .cap = sv.len};
}

sb_t sb_from_cstr(char *cstr) {
  size_t len = strlen(cstr);
  return (sb_t){.ptr = strdup(cstr), .len = len, .cap = len};
}

sb_t sb_from_cstr_slice(char *cstr, size_t len) {
  return (sb_t){.ptr = strndup(cstr, len), .len = len, .cap = len};
}

void sb_reserve(sb_t *sb, size_t amount) {
  if (amount <= sb->cap)
    return;
  sb->ptr = realloc(sb->ptr, amount + SB_EXTRA_CAP);
}

void sb_push_sv(sb_t *sb, sv_t sv) {
  sb_reserve(sb, sb->len + sv.len);
  strncpy(sb->ptr + sb->len, sv.ptr, sv.len);
  sb->len += sv.len;
}

void sb_push_cstr(sb_t *sb, char *cstr) {
  sb_push_sv(sb, sv_make(cstr));
}

void sb_push_ncstr(sb_t *sb, char *cstr, size_t amount) {
  sb_reserve(sb, sb->len + amount);
  strncpy(sb->ptr + sb->len, cstr, amount);
  sb->len += amount;
}

// -- // -- // Temp Buffer // -- // -- //

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
