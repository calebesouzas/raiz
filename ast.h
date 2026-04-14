#ifndef RAIZ_AST_H
#define RAIZ_AST_H

/* format: LEFT << 8 | RIGHT
 * to get LEFT: (PAIR >> 8)
 * to get right: (PAIR & 0xFF)
 * 0xFF == 255 == 11111111 */
#define RZ_PAIR_BP(left_bp, right_bp) (((left_bp) << 8) | (right_bp))
#define RZ_GET_RIGHT_BP(bind_powers) ((uint8_t)((bind_powers) & 0xFF))
#define RZ_GET_LEFT_BP(bind_powers) ((uint8_t)((bind_powers) >> 8))

#define RZ_OP_X_MACRO_TABLE\
  X(RZ_OP_ASSIGN,     RZ_PAIR_BP( 2,  1))\
  X(RZ_OP_EQUAL,      RZ_PAIR_BP( 3,  4))\
  X(RZ_OP_NOT_EQUAL,  RZ_PAIR_BP( 3,  4))\
  X(RZ_OP_BOOL_OR,    RZ_PAIR_BP( 6,  7))\
  X(RZ_OP_BOOL_AND,   RZ_PAIR_BP( 8,  9))\
  X(RZ_OP_GREATER,    RZ_PAIR_BP(10, 11))\
  X(RZ_OP_LESS,       RZ_PAIR_BP(10, 11))\
  X(RZ_OP_GREATER_EQ, RZ_PAIR_BP(10, 11))\
  X(RZ_OP_LESS_EQ,    RZ_PAIR_BP(10, 11))\
  X(RZ_OP_BIT_AND,    RZ_PAIR_BP(12, 13))\
  X(RZ_OP_BIT_OR,     RZ_PAIR_BP(12, 13))\
  X(RZ_OP_BIT_XOR,    RZ_PAIR_BP(12, 13))\
  X(RZ_OP_BIT_RSHIFT, RZ_PAIR_BP(12, 13))\
  X(RZ_OP_BIT_LSHIFT, RZ_PAIR_BP(12, 13))\
  X(RZ_OP_SUM,        RZ_PAIR_BP(14, 15))\
  X(RZ_OP_SUBTRACT,   RZ_PAIR_BP(14, 15))\
  X(RZ_OP_MULTIPLY,   RZ_PAIR_BP(16, 17))\
  X(RZ_OP_DIVIDE,     RZ_PAIR_BP(16, 17))\
  X(RZ_OP_MODULE,     RZ_PAIR_BP(16, 17))\
  X(RZ_OP_BANG,       RZ_PAIR_BP(18, 19))

typedef enum
{
  #define X(variant, power) variant,
  RZ_OP_X_MACRO_TABLE
  #undef X
} Rz_Operator;

// It is good to have a type for some kinds of stuff.
// If we're going to have code which expects a specific kind.
typedef struct
{
  Rz_Operator op;
  size_t left_id, right_id;
} Rz_Expr_Binary;

typedef struct
{
  Rz_Operator op;
  size_t target_id;
} Rz_Expr_Unary;

typedef enum
{
  RZ_EXPR_LITERAL,
  RZ_EXPR_UNARY,
  RZ_EXPR_BINARY,
  RZ_EXPR_VARIABLE,
  RZ_EXPR_VOID,
} Rz_ExprKind;

typedef struct
{
  Rz_ExprKind kind;
  size_t id;
  bool in_arena;
  union
  {
    int literal;
    Rz_Expr_Binary binary;
    Rz_Expr_Unary unary;
    Rz_String variable;
  } as;
} Rz_Expr;

// A dynamic array, but it's not intended to be iterated through directly
typedef struct
{
  size_t count, capacity;
  Rz_Expr *items; // all the 'id' fields in 'Expr' and variants refer to this
} Rz_ExprArena;

uint16_t rz_get_binding_power(Rz_Operator op);
void rz_push_expr(Rz_ExprArena *arena, Rz_Expr *expr);
Rz_Expr *rz_expr_void(Rz_ExprArena *arena);
Rz_Expr *rz_expr_unary(Rz_ExprArena *arena, Rz_Expr *target);
Rz_Expr *rz_expr_literal(Rz_ExprArena *arena, int value);
Rz_Expr *rz_expr_variable(Rz_ExprArena *arena, const char *symbol, size_t size);
Rz_Expr *rz_expr_binary(Rz_ExprArena *arena, Rz_Expr *left, Rz_Expr *right, Rz_Operator op);

#endif // RAIZ_AST_H
