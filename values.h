#ifndef RAIZ_VALUES_H
#define RAIZ_VALUES_H

typedef enum {
  RZ_VALUE_INT,
  RZ_VALUE_FLOAT,
  RZ_VALUE_VIEW,
  RZ_VALUE_STRING,
  RZ_VALUE_BOOL,
} Rz_ValueKind;

typedef struct {
  Rz_ValueKind kind;
  union {
    int integer;
    char character;
    double floating;
    bool boolean;
    // RzView view;
    // RzString string;
  } as;
} Rz_Value;

#endif // RAIZ_VALUES_H
