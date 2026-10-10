#pragma once

#include "token.h"

struct Expr;

typedef struct {
  int value;
} Expr_Literal;

typedef struct {
  struct Expr *inner;
} Expr_Group;

typedef struct {
  struct Expr *inner;
  Token operator;
} Expr_Unary;

typedef struct {
  struct Expr *left, *right;
  Token operator;
} Expr_Binary;

typedef enum {
  EXPR_LITERAL,
  EXPR_GROUP,
  EXPR_UNARY,
  EXPR_BINARY,
} ExprType;

typedef struct Expr {
  union {
    Expr_Literal literal;
    Expr_Group group;
    Expr_Unary unary;
    Expr_Binary binary;
  } as;
  ExprType type;
} Expr;

typedef struct ExprArena {
  Expr *nodes;
  size_t count;
  struct ExprArena *next;
} ExprArena;

typedef struct {
  ExprArena *arena;
  Expr *root;
} Ast;

ExprArena *new_expr_arena(void);
void free_expr_arena(ExprArena *arena);

Expr *new_expr_node(ExprArena *arena);

void dump_ast(Ast *ast, FILE *stream);
