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

static inline Rz_Value rz_value_with_int(int value)
{
  return (Rz_Value) {.kind = RZ_VALUE_INT, .as.integer = value };
}

static inline Rz_Value rz_value_with_double(double value)
{
  return (Rz_Value) {.kind = RZ_VALUE_FLOAT, .as.floating = value };
}

static inline Rz_Value rz_value_with_bool(bool value)
{
  return (Rz_Value) {.kind = RZ_VALUE_BOOL, .as.boolean = value };
}

#define rz_value_with(v) _Generic(v,\
int: rz_value_with_int(v),\
double: rz_value_with_double(v),\
bool: rz_value_with_bool(v))

#endif // RAIZ_VALUES_H
