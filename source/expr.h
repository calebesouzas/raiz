#ifndef RAIZ_EXPR_H
#define RAIZ_EXPR_H

struct Expr;

typedef enum {
  EXPR_LITERAL,
  EXPR_BINARY,
  EXPR_UNARY,
  EXPR_GROUP,
  EXPR_IDENT,
  EXPR_DECL,
  EXPR_BLOCK,
  EXPR_PARENT,
  EXPR_IF,
  EXPR_WHILE,
  EXPR_BREAK,
  EXPR_CONTINUE,
  EXPR_PRINT,
  EXPR_READ,
} ExprKind;

typedef struct {
  Token *op;
  struct Expr *ls;
  struct Expr *rs;
} Expr_Binary;

typedef struct {
  Token *op;
  struct Expr *in;
} Expr_Unary;

typedef struct {
  struct Expr *in;
} Expr_Group;

typedef struct {
  Token *ident;
  struct Expr *value;
  TypePattern type;
} Expr_Decl;

typedef struct {
  struct Expr **dat;
  size_t len, cap;
} Expr_Block;

typedef struct {
  uint32_t level;
  Token *ident;
} Expr_Parent;

typedef struct {
  struct Expr *cond, *then_branch, *else_branch;
} Expr_If_node;

typedef struct {
  struct Expr *cond, *body, *then_branch, *else_branch;
} Expr_While_node;

typedef struct {
  struct Expr *value;
} Expr_Print;

typedef struct Expr {
  ExprKind kind;
  union {
    Token *literal;
    Expr_Binary binary;
    Expr_Unary unary;
    Expr_Group group;
    Token *ident;
    Expr_Decl decl;
    Expr_Block block;
    Expr_Parent parent;
    Expr_If_node if_node;
    Expr_While_node while_node;
    Expr_Print print;
  };
  Token *token;
} Expr;
da_make(ExprNode_A, Expr**);

#endif // RAIZ_EXPR_H
