#ifndef RAIZ_PARSER_SOURCE
#define RAIZ_PARSER_SOURCE

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

uint16_t get_binding_power(Operator op)
{
  uint16_t powers[] = {
    #define X(variant, binding_power) binding_power,
    OP_X_MACRO_TABLE
    #undef X
  };

  return powers[op];
}

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


#define lexer_format_token(token) (token).len, (token).lexeme

Token lexer_next(Lexer *lexer)
{
  if (lexer->current >= lexer->end) return (Token) { .kind = TOKEN_EOF };

  while (isspace(lexer->source[lexer->current])) lexer->current++;

  switch (lexer->source[lexer->current++])
  {
    case '\0':return (Token) {.kind = TOKEN_EOF };
    case '(': return (Token) {.kind = TOKEN_LPAREN, };
    case ')': return (Token) {.kind = TOKEN_RPAREN, };
    case '+': return (Token) {.kind = TOKEN_OP, .as.op = OP_SUM };
    case '-': return (Token) {.kind = TOKEN_OP, .as.op = OP_SUBTRACT };
    case '*': return (Token) {.kind = TOKEN_OP, .as.op = OP_MULTIPLY };
    case '/': return (Token) {.kind = TOKEN_OP, .as.op = OP_DIVIDE };
    case '%': return (Token) {.kind = TOKEN_OP, .as.op = OP_MODULE };
    case '=':
      if (lexer->source[lexer->current] == '=')
        return (Token) {.kind = TOKEN_OP, .as.op = OP_EQUAL };
      return (Token) {.kind = TOKEN_OP, .as.op = OP_ASSIGN };
    case '!':
      if (lexer->source[lexer->current] != '=')
        PANIC("lexer_next(): no support for function explosions yet\n");
      return (Token) {.kind = TOKEN_OP, .as.op = OP_NOT_EQUAL };
    case '>':
      if (lexer->source[lexer->current] == '=')
        return (Token) {.kind = TOKEN_OP, .as.op = OP_GREATER_EQ };
      else if (lexer->source[lexer->current] == '>')
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_RSHIFT };
      return (Token) {.kind = TOKEN_OP, .as.op = OP_GREATER };
    case '<':
      if (lexer->source[lexer->current] == '=')
        return (Token) {.kind = TOKEN_OP, .as.op = OP_LESS_EQ };
      else if (lexer->source[lexer->current] == '<')
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_LSHIFT };
      return (Token) {.kind = TOKEN_OP, .as.op = OP_LESS };
    case '^':
      return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_XOR };
    case '|':
      if (lexer->source[lexer->current] == '|')
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BOOL_OR };
      return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_OR };
    case '&':
      if (lexer->source[lexer->current] == '&')
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BOOL_AND };
      return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_AND };
    default:
      if (isdigit(lexer->source[--lexer->current]))
      {
        int number = lexer->source[lexer->current] - '0';
        while (isdigit(lexer->source[lexer->current])
          number = (number * 10) + (lexer->source[lexer->current] - '0');

        return (Token) { .kind = TOKEN_INT, .as.literal = number };
      }
      else if (isalpha(lexer->source[--lexer->current]))
      {
        while (isalnum(lexer->source[lexer->current)) lexer->current++;
        return (Token) {.kind = TOKEN_IDENT };
      }
      PANIC("lexer_next(): invalid token at index %u: '%c'\n",
            lexer->current, lexer->source[lexer->current]);
  }
  return (Token) { .kind = TOKEN_ERROR };
}

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


typedef struct
{
  size_t count, capacity;
  Expr *items;
} ExprArena;


void push_expr(ExprArena *arena, Expr *expr)
{
  if (arena->capacity == 0)
  {
    arena->capacity = 256;
    arena->items = malloc(sizeof(Expr) * arena->capacity);
  }
  if (arena->count >= arena->capacity)
  {
    arena->capacity *= 2;
    arena->items = realloc(arena->items, sizeof(Expr) * arena->capacity);
  }
  if (!expr->in_arena && arena->items != NULL)
  {
    expr->id = arena->count;
    arena->items[arena->count++] = *expr;
    expr->in_arena = true;
  }
}


Expr *expr_binary(ExprArena *arena, Expr *left, Expr *right, Operator op)
{
  Expr result = (Expr) {
    .kind = EXPR_BINARY,
    .as.binary = (Expr_Binary) {
      .left_id = left->id,
      .right_id = right->id,
      .op = op,
    }
  };
  push_expr(arena, &result);
  assert(result.in_arena);
  return &arena->items[result.id];
}

Expr *expr_unary(ExprArena *arena, Expr *target)
{
  Expr result = (Expr) {
    .kind = EXPR_UNARY,
    .as.unary = (Expr_Unary) {
      .op = OP_SUBTRACT,
      .target_id = target->id,
    }
  };
  push_expr(arena, &result);
  assert(result.in_arena);
  return &arena->items[result.id];
}

Expr *expr_literal(ExprArena *arena, int value)
{
  Expr result = (Expr) {
    .kind = EXPR_LITERAL,
    .as.literal = value,
  };
  push_expr(arena, &result);
  assert(result.in_arena);
  return &arena->items[result.id];
}

typedef struct
{
  Lexer *lexer;
  Token current;
  Token next;
  ExprArena *arena;
  size_t callno; // for debugging purpose
  size_t depth; // for limits and debugging purpose
} Parser;

void parser_advance(Parser *parser, TokenKind expected_current_kind)
{
  ASSERT_EQ(parser->current.kind, expected_current_kind);
  parser->current = parser->next;
  parser->next = lexer_next(parser->lexer);
}

static inline Token parser_peek(Parser *parser)
{
  return parser->next;
}

Parser parser_new(Lexer *lexer, ExprArena *arena)
{
  Parser parser = {0};
  parser.lexer = lexer;
  parser.arena = arena;
  // bootstrap 'parser.next'
  parser_advance(&parser, TOKEN_ERROR);
  // fill 'parser.current' and update 'parser.next'
  parser_advance(&parser, TOKEN_ERROR);
  return parser;
}

Expr *parser_parse_expr(Parser *parser, uint8_t min_bp);

Expr *parser_parse_nud(Parser *parser)
{
  if (parser->current.kind == TOKEN_OP && parser->current.as.op == OP_SUBTRACT)
  {
    uint8_t bind_power = GET_RIGHT_BP(get_binding_power(parser->current.as.op));
    parser_advance(parser, TOKEN_OP);
    return expr_unary(parser->arena, parser_parse_expr(parser, bind_power));
  }
  else if (parser->current.kind == TOKEN_LPAREN)
  {
    parser_advance(parser, TOKEN_LPAREN);
    Expr *result = parser_parse_expr(parser, 0);
    if (parser->current.kind != TOKEN_RPAREN)
    {
      PANIC("parser_parse_nud(): expected ')' (found: %s, call: #%u)\n",
            lexer_format_token(parser->next), parser->callno);
    }
    parser_advance(parser, TOKEN_RPAREN);
    return result;
  }
  else if (parser->current.kind != TOKEN_INT)
  {
    PANIC("parser_parse_nud(): expected number token (found %s, call: #%u)\n",
          lexer_format_token(parser->current), parser->callno
    );
  }

  int lit = parser->current.as.literal;
  parser_advance(parser, TOKEN_INT);
  return expr_literal(parser->arena, lit);
}

Expr *parser_parse_expr(Parser *parser, uint8_t min_bp)
{
  Expr *left_side = parser_parse_nud(parser);

  while (parser->next.kind != TOKEN_EOF && parser->current.kind != TOKEN_RPAREN)
  {
    if (parser->current.kind != TOKEN_OP)
    {
      PANIC("parser_parse_expr(): expected operator (found: %s, call: #%u)\n",
            lexer_format_token(parser->current), parser->callno);
    }

    Operator op = parser->current.as.op;

    uint16_t bps = get_binding_power(op);
    if (GET_LEFT_BP(bps) < min_bp)
    {
      break;
    }
    parser_advance(parser, TOKEN_OP);

    Expr *right_side = parser_parse_expr(parser, GET_RIGHT_BP(bps));

    left_side = expr_binary(parser->arena, left_side, right_side, op);
  }

  parser->depth--;
  return left_side;
}

#endif // RAIZ_PARSER_SOURCE
