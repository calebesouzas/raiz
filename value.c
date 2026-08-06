#ifndef RAIZ_VALUE_C
#define RAIZ_VALUE_C

char *Value_string(Value *v) {
  char rest[32];
  switch (v->kind) {
  case VAL_INT:
    snprintf(rest, sizeof(rest), "%d", (int)v->data);
    break;
  case VAL_UINT:
    snprintf(rest, sizeof(rest), "%u", (unsigned int)v->data);
    break;
  case VAL_BYTE:
    snprintf(rest, sizeof(rest), "%hhu", (unsigned char)v->data);
    break;
  case VAL_BOOL:
    snprintf(rest, sizeof(rest), "%s", v->data ? "true" : "false");
    break;
  case VAL_CHAR:
    snprintf(rest, sizeof(rest), "%c", (char)v->data);
    break;
  case VAL_RAW:
    snprintf(rest, sizeof(rest), "raw[%p]", (void*)v->data);
    break;
  }
  return temp_sprintf(v->flags & VAL_FLAG_PTR ? "&%s" : "%s", rest);
}

char *Value_type(Value *v) {
  switch (v->kind) {
  case VAL_INT: return "int";
  case VAL_UINT: return "nat";
  case VAL_BYTE: return "byte";
  case VAL_BOOL: return "bool";
  case VAL_CHAR: return "char";
  case VAL_RAW: return "raw";
  }
}

#endif // RAIZ_VALUE_C
