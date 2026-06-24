#ifndef RAIZ_RUNTIME_H
#define RAIZ_RUNTIME_H

typedef struct {
  char ident[TOKEN_IDENTIFIER_SIZE];
  int value;
  bool is_variable;
} Symbol;

da_make(Symbol_A, Symbol*);

int eval(Expr *e, Symbol_A *symbols);

#endif // RAIZ_RUNTIME_H
