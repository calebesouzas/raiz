#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

#include "values.h" // Rz_Value

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

typedef enum
{
  RZ_TOKEN_ERROR,
  RZ_TOKEN_EOF,
  RZ_TOKEN_LITERAL,
  RZ_TOKEN_IDENT,
  RZ_TOKEN_OP,
  RZ_TOKEN_LPAREN,
  RZ_TOKEN_RPAREN,
} Rz_TokenKind;

union Rz_TokenData
{
  Rz_Operator op;
  Rz_Value literal;
};

typedef struct
{
  Rz_TokenKind kind;
  union Rz_TokenData as;
  const char *lexeme;
  size_t len;
} Rz_Token;

typedef struct
{
  const char *source;
  size_t start, current, source_len;
} Rz_Lexer;

Rz_Token lexer_next_token(Rz_Lexer *lex);

#endif // RAIZ_LEXER_H
