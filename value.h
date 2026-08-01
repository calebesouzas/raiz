#ifndef RAIZ_VALUE_H
#define RAIZ_VALUE_H

typedef enum {
  VAL_FLAG_PTR = 1<<0,
} ValueFlags;

typedef enum {
  VAL_INT,
  VAL_UINT,
  VAL_BYTE,
  VAL_BOOL,
  VAL_CHAR,
  VAL_RAW,
} ValueKind;

typedef struct {
  uint64_t data;
  ValueKind kind;
  uint8_t flags;
} Value;

#endif // RAIZ_VALUE_C
