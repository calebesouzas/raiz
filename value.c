#ifndef RAIZ_VALUE_C
#define RAIZ_VALUE_C

Value *Value_alloc(Value value) {
  da_add(&Value_Pool, value);
  return da_last(&Value_Pool);
}

#endif /* RAIZ_VALUE_C */
