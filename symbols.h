#ifndef RAIZ_SYMBOLS_H
#define RAIZ_SYMBOLS_H

const int SYM_ANY = 0;

typedef enum {
    SYM_VAR = 1,
    SYM_VAL,
    SYM_TYPE,
} SymbolKind;

typedef struct Symbol {
  sv_t ident;
  SymbolKind kind;
  union {
    Value var;
    Value val;
    Type *type;
  };
} Symbol;

da_make(Symbol_A, Symbol*);

#endif // RAIZ_SYMBOLS_H
