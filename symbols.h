#ifndef RAIZ_SYMBOLS_H
#define RAIZ_SYMBOLS_H

typedef struct {
  char ident[TOKEN_IDENTIFIER_SIZE];
  int value;
  bool is_variable;
} Symbol;

da_make(Symbol_A, Symbol*);

#endif // RAIZ_SYMBOLS_H
