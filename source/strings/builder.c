#include "builder.h"
#include "slice.h"

void
raiz_strb_append(Raiz_StringBuilder *sb, Raiz_String *s) {
  if (sb->capacity == 0) {
    sb->capacity = RAIZ_STRB_CAPACITY;
    sb->items = malloc(sizeof(char) * sb->capacity);
  }

  if (sb->count >= sb->capacity || sb->count + s->size >= sb->capacity) {
    sb->capacity *= 2;
    sb->items = realloc(sb->items, sizeof(char) * sb->capacity);
  }

  if (sb->items != NULL) {
    char *push_addr = sb->items + sb->count > 0? sb->count - 1 : sb->count;

    // @note(safety) will not overflow since `sb->capacity` was already handled
    memcpy(push_addr, s->data, s->size);

    sb->count += s->size;
  }
}

void
raiz_strb_append_cstr(Raiz_StringBuilder *sb, char *cstr) {
  Raiz_String s = raiz_str_from_cstr(cstr);
  raiz_strb_append(sb, &s);
}

Raiz_StringBuilder
raiz_strb_from_cstr(char *cstr) {
  Raiz_StringBuilder sb = {0};
  raiz_strb_append_cstr(&sb, cstr);
  return sb;
}

Raiz_StringBuilder
raiz_str_to_strb(Raiz_String *str) {
  Raiz_StringBuilder sb = {0};
  raiz_strb_append(&sb, str);
  return sb;
}
