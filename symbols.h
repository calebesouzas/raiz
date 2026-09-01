#ifndef RAIZ_SYMBOLS_H
#define RAIZ_SYMBOLS_H

typedef struct {
  sv_t ident;
  enum {
    SYM_VAR,
    SYM_VAL,
    SYM_TYPE,
  } kind;
  union {
    Value var;
    Value val;
    Type *type;
  };
} Symbol;

da_make(Symbol_A, Symbol*);

#endif // RAIZ_SYMBOLS_H
