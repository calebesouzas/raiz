#ifndef RAIZ_SCOPE_H
#define RAIZ_SCOPE_H

typedef struct Scope {
  Symbol_A symbols;
  struct Scope *parent, *inner;
} Scope;

Scope *Scope_(void);
Scope *Scope_new(Scope *parent);
#define Scope_search(sco, sym, len)\
  Scope_search_(sco, sym, len);\
  fprintf(stderr, "searching at %s:%d from %s()\n", __FILE__, __LINE__, __FUNCTION__)
Symbol *Scope_search_(Scope *sco, char *symbol, size_t len);

void Scope_free(Scope *sco);

#endif // RAIZ_SCOPE_H

