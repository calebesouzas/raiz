#include "value.h"

Rz_Value rz_value_int(int value)
{
  return (Rz_Value) {.kind = RZ_VALUE_INT, .as.integer = value };
}

Rz_Value rz_value_char(char value)
{
  return (Rz_Value) {.kind = RZ_VALUE_, .as.character = value };
}

Rz_Value rz_value_bool(bool value)
{
  return (Rz_Value) {.kind = RZ_VALUE_BOOL, .as.boolean = value };
}

Rz_Value rz_value_float(double value)
{
  return (Rz_Value) {.kind = RZ_VALUE_INT, .as.floating = value };
}

Rz_Value rz_value_string(Rz_String value)
{
  return (Rz_Value) {.kind = RZ_VALUE_STRING, .as.string = value };
}
