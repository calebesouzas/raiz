#ifndef RAIZ_PARSER_EXPRESSIONS_H
#define RAIZ_PARSER_EXPRESSIONS_H

/*.This header contains declarations of code related to expressions.
 * It is not intended to be used alone or outside the Raiz parser code
 * */

#include "operators.h"
#include "types.h"

#include <stdbool.h>

typedef enum {
  // Binary operations:
  // should have three fields:
  //// left side: an expression
  //// right side: another expression
  //// operator: a variant of Operator enum
  // detail: are not necessarely arithmethic operations, it can be any kind of
  // operation that uses two operands and an operator
  RAIZ_EXPR_BINARY,
  // Assignments:
  // an exception to the 'rule'(?) at the comment about binary expressions
  // it uses two sides and an operator, but it's more than that.
  // The users may use chained assignments and they should be able to
  // specify whenever they want to assign to a value (immutable)
  // or a variable (mutable) for each item of the chain, for some of the items
  // and even for all of the items. We should also allow type definition
  // (for each item in the chain) instead of always inferring them...
  RAIZ_EXPR_ASSIGN,
  // The check ('?') operator:
  // triggers the 'check' event at an object or expression
  // if an expression:
  //// evaluates that expression and tries to extract a boolean value out of it
  // if an object:
  //// will just trigger the 'check' event at the object and use the boolean
  //// value returned from the event handler (if there is one, else it'll warn)
  RAIZ_EXPR_CHECK,
  // Literals... Very simple, right?
  // They just contain values... But i don't really know how we'll be able to
  // store literals of different types in one single field...
  // I could use an union, but it wouldn't be the best way to do it...
  RAIZ_EXPR_LITERAL,
  // Identifier: alone identifier, without any other operation
  RAIZ_EXPR_IDENTIFIER,
} ExprKind;

typedef struct Expr Expr;

struct Expr {
  ExprKind kind;
  Expr *leftside;
  Expr *rightside;
  struct {
    union {
      char* s_val;
      char c_val[4];
      int i_val;
      double d_val;
    } literal;
    Type* type;
    Operator operator;
    bool mutable;
  } optionals;
};

#endif // RAIZ_PARSER_EXPRESSIONS_H
