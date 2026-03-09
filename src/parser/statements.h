#ifndef RAIZ_PARSER_STATEMENTS_H
#define RAIZ_PARSER_STATEMENTS_H

#include "expressions.h"

typedef enum {
  // Expression statement:
  // an Expression that we don't care about the resulting value.
  // I'm not sure if i should warn when it happens...
  // one field:
  //// expr (of type Expr)
  RAIZ_STMT_EXPRESSION,
  // Variable declaration:
  // instruction starting with 'val' or 'var' keywords.
  // <n> fields:
  //// mutable: bool -> 'true' if used 'var' keyword
  //// symbol: string -> variable name
  //// type: Type* -> optional type defined (if not provided it is inferred)
  //// init: Expr* -> optional expression to evaluate and store the result
  RAIZ_STMT_VAR_DECLARATION,
  RAIZ_STMT_BLOCK,
} StmtKind;

#endif // RAIZ_PARSER_STATEMENTS_H
