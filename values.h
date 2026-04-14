#ifndef RAIZ_VALUES_H
#define RAIZ_VALUES_H

typedef enum {
  RZ_VALUE_INT,
  RZ_VALUE_CHAR,
  RZ_VALUE_BOOL,
  RZ_VALUE_FLOAT,
  RZ_VALUE_STRING,
} Rz_ValueKind;

typedef struct {
  Rz_ValueKind kind;
  union {
    int integer;
    char character;
    double floating;
    bool boolean;
    Rz_String string;
  } as;
} Rz_Value;

Rz_Value rz_value_int(int value);
Rz_Value rz_value_char(char value);
Rz_Value rz_value_bool(bool value);
Rz_Value rz_value_float(double value);
Rz_Value rz_value_string(Rz_String value);

const char *rz_value_extract_kind(Rz_Value value);

#endif // RAIZ_VALUES_H
