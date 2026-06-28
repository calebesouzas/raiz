#ifndef RAIZ_SCOPE_H
#define RAIZ_SCOPE_H

typedef struct Scope {
  Symbol_A symbols;
  struct Scope *parent, *inner;
} Scope;

Scope *Scope_(void);
Scope *Scope_new(Scope *parent);
Symbol *Scope_search(Scope *sco, char *symbol, size_t len);

void Scope_free(Scope *sco);

#endif // RAIZ_SCOPE_H

