#ifndef RAIZ_VALUE_C
#define RAIZ_VALUE_C

Value *Value_alloc(Value value) {
  da_add(&Value_Pool, value);
  return da_last(&Value_Pool);
}

void Value_print(Value *value, bool newline) {
  switch (value->type->kind) {
  case TYPE_char:
    printf("%c", (char)value->data);
    break;
  case TYPE_int:
    printf("%d", (int)value->data);
    break;
  case TYPE_bool:
    printf("%s", value->data? "true" : "false");
    break;
  case TYPE_byte:
    printf("%02x", (uint8_t)value->data);
    break;
  case TYPE_ptr:
    printf("%p", (void*)value->data);
    break;
  }
  if (newline) {
    printf("\n");
  }
}

#endif /* RAIZ_VALUE_C */
