#ifndef RAIZ_SCOPE_H
#define RAIZ_SCOPE_H

typedef struct Scope {
  Symbol_A symbols;
  struct Scope *parent, *inner;
} Scope;

Scope *Scope_(void);
Scope *Scope_new(Scope *parent);
Symbol *Scope_search(Scope *sco, char *symbol, size_t len);

Scope *Scope_next(Scope **sco); // advance and return the new current
Scope *Scope_prev(Scope **sco); // retreat and return the new current

#endif // RAIZ_SCOPE_H

