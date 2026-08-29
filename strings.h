#ifndef RAIZ_STRINGS_H
#define RAIZ_STRINGS_H

struct sb_t;

// -- // -- // String Pool // -- // -- //
struct {
  struct sb_t *dat;
  size_t len, cap;
} String_Pool = {0};

size_t sp_alloc(size_t amount);

size_t sp_put(struct sb_t sb);

size_t sp_put_cstr(char *cstr);

size_t sp_save(char *source);

size_t sp_nsave(char *source, size_t size);

void sp_free(size_t index);

struct sb_t *sp_get(size_t index);

// -- // -- // String View & Builder // -- // -- //
typedef struct {
  char *ptr;
  size_t len;
} sv_t; // string view

typedef struct sb_t {
  char *ptr;
  size_t len, cap;
} sb_t; // string builder (can be cast to `sv_t` in some cases)

sv_t sv_make(char *cstr);

sb_t sb_make(sv_t sv);

sb_t sb_from_cstr(char *cstr);
sb_t sb_from_cstr_slice(char *cstr, size_t len);

// -- // -- // Temp Buffer // -- // -- //
#define TEMP_BUF_CAP 1024
char Temp_Buffer[TEMP_BUF_CAP] = {0};

char *temp_vsnprintf(size_t n, const char *f, va_list v)
  __attribute__((format(printf, 2, 0)));

char *temp_snprintf(size_t n, const char *f, ...)
  __attribute__((format(printf, 2, 3)));

char *temp_vsprintf(const char *f, va_list v)
  __attribute__((format(printf, 1, 0)));

char *temp_sprintf(const char *f, ...)
  __attribute__((format(printf, 1, 2)));

#endif // RAIZ_STRINGS_H
