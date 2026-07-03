#ifndef RAIZ_RUNTIME_H
#define RAIZ_RUNTIME_H

typedef enum {
  SIGNAL_NONE,
  SIGNAL_BREAK,
  SIGNAL_CONTINUE,
} ControlSignal;

typedef struct {
  int value;
  ControlSignal sig;
} EvalResult;

EvalResult eval(Expr *e, Scope *s);

#endif // RAIZ_RUNTIME_H
