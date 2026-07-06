#ifndef RAIZ_VALUE_C
#define RAIZ_VALUE_C

typedef enum {
  TYPE_INT,
  TYPE_UINT,
  TYPE_BYTE,
  TYPE_BOOL,
  TYPE_CHAR,
  TYPE_RAW_PTR,
  TYPE_INT_PTR,
  TYPE_BYTE_PTR,
  TYPE_BOOL_PTR,
  TYPE_CHAR_PTR,
} Type;

bool Type_is_pointer(Type type) {
  switch (type) {
  case TYPE_RAW_PTR:
  case TYPE_INT_PTR:
  case TYPE_BYTE_PTR:
  case TYPE_BOOL_PTR:
  case TYPE_CHAR_PTR:
    return true;
  case TYPE_INT:
  case TYPE_UINT:
  case TYPE_BYTE:
  case TYPE_BOOL:
  case TYPE_CHAR:
    return false;
  }
  UNREACHABLE("Type kind (id %d)\n", type);
  return false;
}

typedef struct {
  Type type;
  union {
    int  v_int;
    uint v_uint;
    byte v_byte;
    bool v_bool;
    char v_char;
    void *v_ptr;
  };
} Value;
  TYPE_INT,
  TYPE_UINT,
  TYPE_BYTE,
  TYPE_BOOL,
  TYPE_CHAR,
  TYPE_RAW_PTR,
  TYPE_INT_PTR,
  TYPE_BYTE_PTR,
  TYPE_BOOL_PTR,
  TYPE_CHAR_PTR,

#define Value_from(v) _Generic((v),\
  int: Value_from_int,\
  uint: Value_from_uint,\
  byte: Value_from_byte,\
  bool: Value_from_bool,\
  char: Value_from_char,\
  int*: Value_from_int_ptr,\
  byte*: Value_from_byte_ptr,\
  bool*: Value_from_bool_ptr,\
  char*: Value_from_char_ptr,\
  default: Value_from_void_ptr\
)

Value Value_from_int(int v) {
  return (Value){.type = TYPE_INT, .v_int = v};
}
Value Value_from_uint(uint v) {
  return (Value){.type = TYPE_UINT, .v_uint = v};
}
Value Value_from_byte(byte v) {
  return (Value){.type = TYPE_BYTE, .v_byte = v};
}
Value Value_from_bool(bool v) {
  return (Value){.type = TYPE_BOOL, .v_bool = v};
}
Value Value_from_char(char v) {
  return (Value){.type = TYPE_CHAR, .v_char = v};
}
Value Value_from_int_ptr(int* v) {
  return (Value){.type = TYPE_INT_PTR, .v_int_ptr = v};
}
Value Value_from_byte_ptr(byte* v) {
  return (Value){.type = TYPE_BYTE_PTR, .v_byte_ptr = v};
}
Value Value_from_bool_ptr(bool* v) {
  return (Value){.type = TYPE_BOOL_PTR, .v_bool_ptr = v};
}
Value Value_from_char_ptr(char* v) {
  return (Value){.type = TYPE_CHAR_PTR, .v_char_ptr = v};
}
Value Value_from_void_ptr(void* v) {
  return (Value){.type = TYPE_VOID_PTR, .v_void_ptr = v};
}

#endif // RAIZ_VALUE_C
