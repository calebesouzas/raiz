#ifndef RAIZ_ERRORS_H
#define RAIZ_ERRORS_H

// TODO: terminate it
#define RZ_DEF_RESULT(type_ok, type_err)\
  typedef struct {\
    enum { RESULT_OK, RESULT_ERR } result;\
    type_ok ok_value;\
    type_err err_value;\
  } Result##type_ok##_##type_err

#endif /* ifndef RAIZ_ERRORS_H */
