#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define DEBUG_MODE
#if defined(DEBUG_MODE) && !defined(DISABLE_LOGS)
#define LOG(...)\
  do\
  {\
    fprintf(stderr, "raiz(info): ");\
    fprintf(stderr, __VA_ARGS__);\
  } while (0)
#else
#define LOG(...)
#endif // DEBUG_MODE and !DISABLE_LOGS

#define PANIC(...)\
  do\
  {\
    fprintf(stderr, "raiz: panicked at %s:%d\n", __FILE__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    abort();\
  } while (0)

#define TODO(message) PANIC("TODO at %s", (message))

#define UNREACHABLE(message) PANIC("reached unreachable code at %s", (message))

#define ASSERT_EQ(left, right)\
  do\
  {\
    if ((left) != (right))\
    {\
      PANIC("assertion failed\n%s != %s\n", #left, #right);\
    }\
  } while (0)

/* format: LEFT << 8 | RIGHT
 * to get LEFT: (PAIR >> 8)
 * to get right: (PAIR & 0xFF)
 * 0xFF == 255 == 11111111 */
#define PAIR_BP(left_bp, right_bp) (((left_bp) << 8) | (right_bp))
#define GET_RIGHT_BP(bind_powers) ((bind_powers) & 0xFF)
#define GET_LEFT_BP(bind_powers) ((bind_powers) >> 8)

#define OP_X_MACRO_TABLE\
  X(OP_EQUAL,      "==", PAIR_BP( 1,  2))\
  X(OP_NOT_EQUAL,  "!=", PAIR_BP( 1,  2))\
  X(OP_BOOL_OR,    "||", PAIR_BP( 2,  3))\
  X(OP_BOOL_AND,   "&&", PAIR_BP( 4,  5))\
  X(OP_BIT_AND,    "&",  PAIR_BP( 6,  7))\
  X(OP_BIT_OR,     "|",  PAIR_BP( 6,  7))\
  X(OP_BIT_XOR,    "^",  PAIR_BP( 6,  7))\
  X(OP_BIT_RSHIFT, ">>", PAIR_BP( 6,  7))\
  X(OP_BIT_LSHIFT, "<<", PAIR_BP( 6,  7))\
  X(OP_GREATER,    ">",  PAIR_BP( 8,  9))\
  X(OP_LESS,       "<",  PAIR_BP( 8,  9))\
  X(OP_GREATER_EQ, ">=", PAIR_BP( 8,  9))\
  X(OP_LESS_EQ,    "<=", PAIR_BP( 8,  9))\
  X(OP_SUM,        "+",  PAIR_BP(10, 11))\
  X(OP_SUBTRACT,   "-",  PAIR_BP(10, 11))\
  X(OP_MULTIPLY,   "*",  PAIR_BP(12, 13))\
  X(OP_DIVIDE,     "/",  PAIR_BP(12, 13))\
  X(OP_MODULE,     "%",  PAIR_BP(12, 13))

typedef enum
{
  #define X(variant, token, power) variant,
  OP_X_MACRO_TABLE
  #undef X
} Operator;

uint16_t get_binding_power(Operator op)
{
  uint16_t powers[] = {
    #define X(variant, token, binding_power) binding_power,
    OP_X_MACRO_TABLE
    #undef X
  };

  return powers[op];
}

#define TOKEN_X_MACRO_TABLE\
  X(TOKEN_ERROR, "<TOKEN_ERROR>")\
  X(TOKEN_EOF, "<TOKEN_EOF>")\
  X(TOKEN_INT, "<TOKEN_INT>")\
  X(TOKEN_OP, "<TOKEN_OP>")\
  X(TOKEN_LPAREN, "<TOKEN_LPAREN>")\
  X(TOKEN_RPAREN, "<TOKEN_RPAREN>")

typedef enum
{
  #define X(variant, string) variant,
  TOKEN_X_MACRO_TABLE
  #undef X
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
} Token;

typedef struct
{
  const char* source;
  size_t current, end; // strlen(source)
} Lexer;

const char* lexer_extract_token(Token token)
{
  // char buffer[512] = {0};
  const char* raiz_tokens[] = {
    #define X(variant, string) string,
    TOKEN_X_MACRO_TABLE
    #undef X
  };
  return raiz_tokens[token.kind];
}

Lexer lexer_new(const char* source)
{
  return (Lexer) { .source = source, .current = 0, .end = strlen(source) };
}

Token lexer_next(Lexer* lexer)
{
  if (lexer->current >= lexer->end) return (Token) { .kind = TOKEN_EOF };

  while (isspace(lexer->source[lexer->current]))
    lexer->current++;

  switch (lexer->source[lexer->current])
  {
    case '(':
      lexer->current++;
      return (Token) {.kind = TOKEN_LPAREN, };
    case ')':
      lexer->current++;
      return (Token) {.kind = TOKEN_RPAREN, };
    case '+':
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_SUM };
    case '-':
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_SUBTRACT };
    case '*':
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_MULTIPLY };
    case '/':
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_DIVIDE };
    case '%':
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_MODULE };
    case '\0':
      return (Token) {.kind = TOKEN_EOF };
    case '=':
      if (lexer->source[lexer->current + 1] != '=')
      {
        PANIC("lexer_next(): no support for assignments yet\n");
      }
      lexer->current += 2;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_EQUAL };
    case '!':
      if (lexer->source[lexer->current + 1] != '=')
      {
        PANIC("lexer_next(): no support for function explosions yet\n");
      }
      lexer->current += 2;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_NOT_EQUAL };
    case '>':
      if (lexer->source[lexer->current + 1] == '=')
      {
        lexer->current += 2;
        return (Token) {.kind = TOKEN_OP, .as.op = OP_GREATER_EQ };
      }
      else if (lexer->source[lexer->current + 1] == '>')
      {
        lexer->current += 2;
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_RSHIFT };
      }
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_GREATER };
    case '<':
      if (lexer->source[lexer->current + 1] == '=')
      {
        lexer->current += 2;
        return (Token) {.kind = TOKEN_OP, .as.op = OP_LESS_EQ };
      }
      else if (lexer->source[lexer->current + 1] == '<')
      {
        lexer->current += 2;
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_LSHIFT };
      }
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_LESS };
    case '^':
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_XOR };
    case '|':
      if (lexer->source[lexer->current + 1] == '|')
      {
        lexer->current += 2;
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BOOL_OR };
      }
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_OR };
    case '&':
      if (lexer->source[lexer->current + 1] == '&')
      {
        lexer->current += 2;
        return (Token) {.kind = TOKEN_OP, .as.op = OP_BOOL_AND };
      }
      lexer->current++;
      return (Token) {.kind = TOKEN_OP, .as.op = OP_BIT_AND };
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      ;
      int number = 0;
      for (; isdigit(lexer->source[lexer->current]); lexer->current++)
        number = (number * 10) + (lexer->source[lexer->current] - '0');

      return (Token) { .kind = TOKEN_INT, .as.literal = number };
    default:
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
  Expr* items;
} ExprArena;


void push_expr(ExprArena* arena, Expr* expr)
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
    LOG("push_expr(): pushing expression #%u\n", expr->id);
  }
}


Expr* expr_binary(ExprArena* arena, Expr* left, Expr* right, Operator op)
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

Expr* expr_unary(ExprArena* arena, Expr* target)
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

Expr* expr_literal(ExprArena* arena, int value)
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
  Lexer* lexer;
  Token current;
  Token next;
  ExprArena* arena;
  size_t callno; // for debugging purpose
  size_t depth; // for limits and debugging purpose
} Parser;

void parser_advance(Parser* parser, TokenKind expected_current_kind)
{
  ASSERT_EQ(parser->current.kind, expected_current_kind);
  LOG("------------------------------------\n");
  parser->current = parser->next;
  parser->next = lexer_next(parser->lexer);

  LOG("parser_advance(): new current = %s\n",
      lexer_extract_token(parser->current));
  LOG("parser_advance(): new next = %s\n",
      lexer_extract_token(parser->next));
  LOG("------------------------------------\n");
}

static inline Token parser_peek(Parser* parser)
{
  return parser->next;
}

Parser parser_new(Lexer* lexer, ExprArena* arena)
{
  Parser parser = {0};
  parser.lexer = lexer;
  parser.arena = arena;
  LOG("parser_new(): initializing parser...\n");
  // bootstrap 'parser.next'
  parser_advance(&parser, TOKEN_ERROR);
  // fill 'parser.current' and update 'parser.next'
  parser_advance(&parser, TOKEN_ERROR);
  return parser;
}

Expr* parser_parse_expr(Parser* parser, uint8_t min_bp);

Expr* parser_parse_nud(Parser* parser)
{
  if (parser->current.kind == TOKEN_OP && parser->current.as.op == OP_SUBTRACT)
  {
    LOG("parser_parse_nud(): mounting unary\n");
    uint8_t bind_power = GET_RIGHT_BP(get_binding_power(parser->current.as.op));
    parser_advance(parser, TOKEN_OP);
    return expr_unary(parser->arena, parser_parse_expr(parser, bind_power));
  }
  else if (parser->current.kind == TOKEN_LPAREN)
  {
    LOG("parser_parse_nud(): mounting group\n");
    parser_advance(parser, TOKEN_LPAREN);
    Expr *result = parser_parse_expr(parser, 0);
    if (parser->current.kind != TOKEN_RPAREN)
    {
      PANIC("parser_parse_nud(): expected ')' (found: %s, call: #%u)\n",
            lexer_extract_token(parser->next), parser->callno);
    }
    parser_advance(parser, TOKEN_RPAREN);
    return result;
  }
  else if (parser->current.kind != TOKEN_INT)
  {
    PANIC("parser_parse_nud(): expected number token (found %s, call: #%u)\n",
          lexer_extract_token(parser->current), parser->callno
    );
  }
  LOG("parser_parse_nud(): returning literal\n");

  int lit = parser->current.as.literal;
  parser_advance(parser, TOKEN_INT);
  return expr_literal(parser->arena, lit);
}

Expr* parser_parse_expr(Parser* parser, uint8_t min_bp)
{
  LOG("parser_parse_expr(): call #%u, depth %u\n",
      ++parser->callno, ++parser->depth);
  Expr* left_side = parser_parse_nud(parser);

  while (parser->next.kind != TOKEN_EOF && parser->current.kind != TOKEN_RPAREN)
  {
    if (parser->current.kind != TOKEN_OP)
    {
      PANIC("parser_parse_expr(): expected operator (found: %s, call: #%u)\n",
            lexer_extract_token(parser->current), parser->callno);
    }

    Operator op = parser->current.as.op;

    uint8_t bps = get_binding_power(op);
    if (GET_LEFT_BP(bps) < min_bp)
    {
      LOG("parser_parse_expr(): breaking loop\n");
      break;
    }
    parser_advance(parser, TOKEN_OP);

    LOG("parser_parse_expr(): mounting right side\n");
    Expr* right_side = parser_parse_expr(parser, GET_RIGHT_BP(bps));

    LOG("parser_parse_expr(): re-mounting left side\n");
    left_side = expr_binary(parser->arena, left_side, right_side, op);
  }

  parser->depth--;
  return left_side;
}

Expr* parser_build_ast(ExprArena* arena, const char* source)
{
  Lexer lexer = lexer_new(source);
  Parser parser = parser_new(&lexer, arena);
  return parser_parse_expr(&parser, 0);
}

int eval(ExprArena* arena, size_t current)
{
  switch (arena->items[current].kind)
  {
    case EXPR_LITERAL: return arena->items[current].as.literal;
    case EXPR_UNARY:
      if (arena->items[current].as.unary.op != OP_SUBTRACT)
        PANIC("use unary with '-' operator\n");

      current = arena->items[current].as.unary.target_id;
      return -eval(arena, current);
    case EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      int left = eval(arena, arena->items[current].as.binary.left_id);
      int right = eval(arena, arena->items[current].as.binary.right_id);

      switch (arena->items[current].as.binary.op)
      {
        case OP_SUM:        return left + right;
        case OP_SUBTRACT:   return left - right;
        case OP_MULTIPLY:   return left * right;
        case OP_DIVIDE:     return left / right;
        case OP_MODULE:     return left % right;
        case OP_GREATER:    return left > right;
        case OP_LESS:       return left < right;
        case OP_BIT_OR:     return left | right;
        case OP_BIT_XOR:    return left ^ right;
        case OP_BIT_AND:    return left & right;
        case OP_BIT_RSHIFT: return left >> right;
        case OP_BIT_LSHIFT: return left << right;
        case OP_EQUAL:      return left == right;
        case OP_NOT_EQUAL:  return left != right;
        case OP_GREATER_EQ: return left >= right;
        case OP_LESS_EQ:    return left <= right;
        case OP_BOOL_AND:   return left && right;
        case OP_BOOL_OR:    return left || right;
        default: UNREACHABLE("eval(): operator\n");
      }
    default: // switch (arena->items[current].kind)
      UNREACHABLE("eval(): expression kind\n");
  }
}

void dump_ast(ExprArena* arena, Expr* ast, size_t level)
{
  for (size_t i = 0; i < level; i++) printf("  ");

  char* operators[] = {
    #define X(variant, token, bind_power) token,
    OP_X_MACRO_TABLE
    #undef X
  };

  switch (arena->items[ast->id].kind)
  {
    case EXPR_LITERAL:
      printf("Literal %d\n", ast->as.literal);
      break;
    case EXPR_UNARY:
      printf("Unary %s\n", operators[ast->as.unary.op]);
      dump_ast(arena, &arena->items[ast->as.unary.target_id], level + 1);
      break;
    case EXPR_BINARY:
      ; // HACK this thing literally made "declaration after label is a C23
        // extension" compiler warning disappear!
      printf("Binary %s\n", operators[ast->as.binary.op]);
      dump_ast(arena, &arena->items[ast->as.binary.left_id],  level + 1);
      dump_ast(arena, &arena->items[ast->as.binary.right_id], level + 1);
      break;

    default: // switch (arena->items[current].kind)
      UNREACHABLE("dump_ast(): expression kind\n");
  }
}

int eval_arena(ExprArena* arena)
{
  return eval(arena, arena->count - 1);
}

int main(void)
{
  char buffer[1024] = {0};

  printf("$> ");
  while (fgets(buffer, sizeof(buffer), stdin))
  {
    if (strncmp(buffer, "exit", 4) == 0) break;

    ExprArena arena = {0};
    Expr* ast = parser_build_ast(&arena, buffer);

    printf("---- AST ----\n");
    dump_ast(&arena, ast, 1);

    printf("Result %d\n", eval_arena(&arena));

    free(arena.items);
    printf("$> ");
  }
  return 0;
}
