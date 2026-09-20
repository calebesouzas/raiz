#ifndef RAIZ_SCOPE_H
#define RAIZ_SCOPE_H

typedef struct Scope {
  Symbol_A symbols;
  struct Scope *parent, *inner;
} Scope;

Scope *Scope_(void);
Scope *Scope_copy(Scope *parent);
Scope *Scope_new(Scope *parent);
void Scope_insert_builtins(Scope *sco);

void Scope_free(Scope *sco);
void Scope_dump(Scope *sco);

#endif // RAIZ_SCOPE_H

