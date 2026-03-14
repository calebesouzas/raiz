#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

#include "lexer.h"
#include <stdbool.h>

/* This header contains type definitions and function declarations 
 * for the Raiz parser. It does not contain any implementation!
 * There are some things that i need to do. Like: think more about what the
 * syntax should look like. It should not complicate users' lifes but not 
 * complicate our parser also...
 * About the parser, i'm really thinking on playing with something like
 * a reverse or hybrid thing... But i don't know how i'll do the hybrid part...
 * That would be great since we won't need recursive calls for 'looking ahead'
 * */

typedef enum {
  // Binary operations:
  // should have three fields:
  //// left side: an expression
  //// right side: another expression
  //// operator: a variant of Operator enum
  // detail: are not necessarely arithmethic operations, it can be any kind of
  // operation that uses two operands and an operator
  RAIZ_EXPR_BINARY,
  RAIZ_EXPR_NEGATION, // logical NOT or unary with '-' operator
  // Assignments:
  // an exception to the 'rule'(?) at the comment about binary expressions
  // it uses two sides and an operator, but it's more than that.
  // The users may use chained assignments and they should be able to
  // specify whenever they want to assign to a value (immutable)
  // or a variable (mutable) for each item of the chain, for some of the items
  // and even for all of the items. We should also allow type definition
  // (for each item in the chain) instead of always inferring them...
  RAIZ_EXPR_ASSIGN,
  // I could use 'if' expressions and reserve the '?' for something else...
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
  RAIZ_EXPR_TYPE_IDENTIFIER,
  RAIZ_EXPR_CALLABLE,
} Raiz_ExprKind;

typedef enum {
  RAIZ_OP_SUM,
  RAIZ_OP_SUBTRACT,
  RAIZ_OP_MULTIPLY,
  RAIZ_OP_DIVIDE,
  RAIZ_OP_EQUAL,
  RAIZ_OP_NOT_EQUAL,
  RAIZ_OP_GREATER,
  RAIZ_OP_LESSER,
  RAIZ_OP_GREAT_OR_EQUAL,
  RAIZ_OP_LESS_OR_EQUAL,
  RAIZ_OP_TYPE_CAST,
  RAIZ_OP_BANG,
} Raiz_Op;

typedef struct Raiz_Expr Raiz_Expr;

// TODO: finish types!
typedef enum {
  RAIZ_TYPE_SINGLE,   // val x @int = 15
  RAIZ_TYPE_TUPLE,    // val pair @(@string, @int) = ("Calebe", 16)
  RAIZ_TYPE_ARRAY,    // var scores @[@float] = [7.5, 8, 6.7, 9.8, 10]
  RAIZ_TYPE_PACK,     // val rooms @[@int, 10] = [1..=10]
  RAIZ_TYPE_DICT, // var locales @{string, @string}
  RAIZ_TYPE_NESTED,   // TODO: think on a good example of nested types
  // NOTE: there should be a depth limit for nested types!
} Raiz_TypeKind;

typedef struct Raiz_Type Raiz_Type;

struct Raiz_Type {
  Raiz_TypeKind kind;
  union {
    struct {char *name;} single;
    struct {
      uint8_t count;
      Raiz_Type* items;
    } tuple;
    struct {
      Raiz_Type* inner;
    } nested;
    Raiz_Type* array;
    struct {
      Raiz_Type* type;
      size_t count;
    } pack;
    struct {
      Raiz_Type* keys;
      Raiz_Type* values;
    } dict;
  } as;
};

// TODO: refactor it preparing for THAT syntax!
struct Raiz_Expr {
  Raiz_ExprKind kind;
  union {
    struct {
      Raiz_Expr* lhs;
      Raiz_Expr* rhs;
      Raiz_Op op;
    } binary;
    struct {
      char* s_val;
      int i_val;
      char c_val;
    } literal;
    Raiz_Type type; // not sure about type expressions
  } as;
  bool is_mutable;
};

typedef struct {
  size_t index; // it causes some issues when i use my phone because it's 32bit
  Raiz_Token* tokens;
  Raiz_Expr* ast;
} Raiz_ParserState;

#endif // RAIZ_PARSER_H
