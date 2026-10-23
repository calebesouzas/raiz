#pragma once

#include "token.h"
#include "generics.h"

struct Expr;

typedef struct {
  int value;
} Expr_Literal;

typedef Token * Expr_Identifier;

typedef struct {
  struct Expr *inner;
} Expr_Group;

typedef struct {
  struct Expr *inner;
  Token *operator;
} Expr_Unary;

typedef struct {
  struct Expr *left, *right;
  Token *operator;
} Expr_Binary;

typedef array_of(struct Expr*) Expr_Block;

typedef struct {
  Expr_Identifier identifier;
  struct Expr *value;
} Expr_Declaration;

typedef enum {
  EXPR_LITERAL,
  EXPR_GROUP,
  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_IDENTIFIER,
  EXPR_BLOCK,
  EXPR_DECLARATION,
} ExprType;

typedef struct Expr {
  union {
    Expr_Literal literal;
    Expr_Group group;
    Expr_Unary unary;
    Expr_Binary binary;
    Expr_Identifier identifier;
    Expr_Block block;
    Expr_Declaration declaration;
  } as;
  size_t id;
  ExprType type;
} Expr;

typedef array_of(Expr) ExprArray;

typedef struct {
  bool ok;
  ExprArray pool;
  Expr *root;
} Ast;

void dump_ast(Ast *ast, FILE *stream);
