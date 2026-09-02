#ifndef RAIZ_SYMBOLS_H
#define RAIZ_SYMBOLS_H

typedef enum {
    SYM_VAR,
    SYM_VAL,
    SYM_TYPE,
} SymbolKind;

typedef struct Symbol {
  sv_t ident;
  SymbolKind kind;
  union {
    Value var;
    Value val;
    const Type *type;
  };
} Symbol;

da_make(Symbol_A, Symbol*);

#endif // RAIZ_SYMBOLS_H
