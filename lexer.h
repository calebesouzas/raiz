#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

/* format: LEFT << 8 | RIGHT
 * to get LEFT: (PAIR >> 8)
 * to get right: (PAIR & 0xFF)
 * 0xFF == 255 == 11111111 */
#define PAIR_BP(left_bp, right_bp) (((left_bp) << 8) | (right_bp))
#define GET_RIGHT_BP(bind_powers) ((uint8_t)((bind_powers) & 0xFF))
#define GET_LEFT_BP(bind_powers) ((uint8_t)((bind_powers) >> 8))

#define OP_X_MACRO_TABLE\
  X(OP_ASSIGN,     PAIR_BP( 2,  1))\
  X(OP_EQUAL,      PAIR_BP( 3,  4))\
  X(OP_NOT_EQUAL,  PAIR_BP( 3,  4))\
  X(OP_BOOL_OR,    PAIR_BP( 6,  7))\
  X(OP_BOOL_AND,   PAIR_BP( 8,  9))\
  X(OP_GREATER,    PAIR_BP(10, 11))\
  X(OP_LESS,       PAIR_BP(10, 11))\
  X(OP_GREATER_EQ, PAIR_BP(10, 11))\
  X(OP_LESS_EQ,    PAIR_BP(10, 11))\
  X(OP_BIT_AND,    PAIR_BP(12, 13))\
  X(OP_BIT_OR,     PAIR_BP(12, 13))\
  X(OP_BIT_XOR,    PAIR_BP(12, 13))\
  X(OP_BIT_RSHIFT, PAIR_BP(12, 13))\
  X(OP_BIT_LSHIFT, PAIR_BP(12, 13))\
  X(OP_SUM,        PAIR_BP(14, 15))\
  X(OP_SUBTRACT,   PAIR_BP(14, 15))\
  X(OP_MULTIPLY,   PAIR_BP(16, 17))\
  X(OP_DIVIDE,     PAIR_BP(16, 17))\
  X(OP_MODULE,     PAIR_BP(16, 17))

typedef enum
{
  #define X(variant, power) variant,
  OP_X_MACRO_TABLE
  #undef X
} Operator;

typedef enum
{
  TOKEN_ERROR,
  TOKEN_EOF,
  TOKEN_LIT_INT,
  TOKEN_OP,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
} TokenKind;

union TokenData
{
  Operator op;
  int literal;
};

typedef struct
{
  TokenKind kind;
  union TokenData as;
  char *lexeme;
  size_t len;
} Token;

typedef struct
{
  const char *source;
  size_t current, source_len;
} Lexer;

#endif // RAIZ_LEXER_H
