#include "value.h"

Rz_Value rz_value_int(int value) { return (Rz_Value) {.kind = RZ_VALUE_INT, .as.integer = value }; }

Rz_Value rz_value_char(char value) { return (Rz_Value) {.kind = RZ_VALUE_, .as.character = value }; }

Rz_Value rz_value_bool(bool value) { return (Rz_Value) {.kind = RZ_VALUE_BOOL, .as.boolean = value }; }

Rz_Value rz_value_void(void) { return (Rz_Value) {.kind = RZ_VALUE_VOID }; }

Rz_Value rz_value_float(double value) { return (Rz_Value) {.kind = RZ_VALUE_INT, .as.floating = value }; }

Rz_Value rz_value_string(Rz_String value) { return (Rz_Value) {.kind = RZ_VALUE_STRING, .as.string = value }; }

const char *rz_value_extract_kind(Rz_Value value)
{
  switch (value.kind)
  {
    case RZ_VALUE_INT:    return "int";
    case RZ_VALUE_CHAR:   return "char";
    case RZ_VALUE_BOOL:   return "bool";
    case RZ_VALUE_FLOAT:  return "float";
    case RZ_VALUE_STRING: return "string";
    case RZ_VALUE_VOID:   return "void";
  }
}
