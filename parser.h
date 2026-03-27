#ifndef RAIZ_PARSER_H
#define RAIZ_PARSER_H

#include "raiz.h"

// It is good to have a type for some kinds of stuff.
// If we're going to have code which expects a specific kind.
typedef struct
{
  Operator op;
  size_t left_id, right_id;
} Expr_Binary;

typedef struct
{
  Operator op;
  size_t target_id;
} Expr_Unary;

typedef enum
{
  EXPR_LITERAL,
  EXPR_UNARY,
  EXPR_BINARY,
} ExprKind;

typedef struct
{
  ExprKind kind;
  size_t id;
  bool in_arena;
  union
  {
    int literal;
    Expr_Binary binary;
    Expr_Unary unary;
  } as;
} Expr;

// A dynamic array, but it's not intended to be iterated through directly
typedef struct
{
  size_t count, capacity;
  Expr *items; // all the 'id' fields in 'Expr' and variants refer to this
} ExprArena;

typedef struct
{
  Lexer *lexer;
  Token current;
  Token next;
  ExprArena *arena;
  // Do we really need it? I think it's better than 'PANIC()'ing everywhere...
  char error_buffer[RAIZ_COLUMN_LIMIT + RAIZ_COLUMN_LIMIT / 2];
} Parser;

/*** Some helpers again, why not? ***/

/* returns the current token */
static inline Token peek(Parser *par);

/* just returns the next token */
static inline Token next(Parser *par);

/* sets 'current' to next, gets a new 'next' and returns 'current' */
static inline Token advance(Parser *par);

/* checks if 'current' is 'expected' */
static inline bool match(Parser *par, TokenKind expected);

/* same as 'match()' but now we expect operators */
static inline bool match_op(Parser *par, Operator expected);

/* check if the next token's kind is 'expected' */
static inline bool match_next(Parser *par, TokenKind expected);

/* check if the next token is an operator and the same as 'expected' */
static inline bool match_next_op(Parser *par, Operator expected);

#endif // RAIZ_PARSER_H
