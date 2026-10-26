#pragma once

#include "token.h"
#include "generics.h"

struct Expr;

typedef struct {
  int value;
} Expr_Literal;

typedef Token * Expr_Identifier;

typedef struct {
  size_t inner_id;
} Expr_Group;

typedef struct {
  size_t inner_id;
  Token *operator;
} Expr_Unary;

typedef struct {
  size_t left_id, right_id;
  Token *operator;
} Expr_Binary;

typedef array_of(size_t) Expr_Block;

typedef struct {
  Expr_Identifier identifier;
  size_t value_id;
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
  Span span;
} Expr;

typedef array_of(Expr) ExprArray;

typedef struct {
  bool ok;
  ExprArray pool;
  Expr *root;
} Ast;

typedef void (*WalkerFunction) (Expr *node, unsigned int depth, void *data, void *out);
void ast_walk(Ast *ast, WalkerFunction visit, bool post_order, void *data, void *out);

void dump_ast(Ast *ast, FILE *stream);
