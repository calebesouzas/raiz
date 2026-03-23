#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define PANIC(...)\
  do\
  {\
    fprintf(stderr, "raiz: panicked at %s:%d\n", __FILE__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    abort();\
  } while (0)

#define TODO(message) PANIC("TODO at %s", (message))

#define UNREACHABLE(message) PANIC("reached unreachable code at %s", (message))

typedef enum
{
  OP_SUM,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
} Operator;

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
    case '\0':
      return (Token) {.kind = TOKEN_EOF };
    default:
      ;
      int number = 0;
      for (; isdigit(lexer->source[lexer->current]); lexer->current++)
        number = (number * 10) + (lexer->source[lexer->current] - '0');

      return (Token) { .kind = TOKEN_INT, .as.literal = number };
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

#define GET_RIGHT_BP(bind_powers) ((bind_powers) & 0x0f) // 16
#define GET_LEFT_BP(bind_powers) ((bind_powers) >> 4)

uint8_t get_binding_power(Operator op)
{
  switch (op)
  {
    case OP_SUM:
    case OP_SUBTRACT:
      // format: RIGHT + (LEFT << 4)
      return 2 + (1 << 4);
    case OP_MULTIPLY:
    case OP_DIVIDE:
      return 3 + (4 << 4);
    default: UNREACHABLE("get_binding_power(): operator");
  }
}

typedef struct
{
  Lexer* lexer;
  Token current;
  Token next;
  ExprArena* arena;
  size_t callno; // for debugging purpose
} Parser;

void parser_advance(Parser* parser)
{
  parser->current = parser->next;
  parser->next = lexer_next(parser->lexer);
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
  parser_advance(&parser); // bootstrap 'parser.next'
  parser_advance(&parser); // fill 'parser.current' and update 'parser.next'
  return parser;
};

Expr* parser_parse_expr(Parser* parser, uint8_t min_bp);

Expr* parser_parse_nud(Parser* parser)
{
  if (parser->current.kind == TOKEN_OP && parser->current.as.op == OP_SUBTRACT)
  {
    uint8_t bind_power = GET_LEFT_BP(get_binding_power(parser->current.as.op));
    parser_advance(parser); // consume operator token
    return expr_unary(parser->arena, parser_parse_expr(parser, bind_power));
  }
  else if (parser->current.kind == TOKEN_LPAREN)
  {
    parser_advance(parser); // consume '('
    Expr *result = parser_parse_expr(parser, 0);
    if (parser->current.kind != TOKEN_RPAREN)
    {
      PANIC("parser_parse_nud(): expected ')'");
    }
    return result;
  }
  else if (parser->current.kind != TOKEN_INT)
  {
    PANIC("parser_parse_nud(): expected number token (found %s, call: #%u)\n",
          lexer_extract_token(parser->current), parser->callno
    );
  }
  return expr_literal(parser->arena, parser->current.as.literal);
}

Expr* parser_parse_expr(Parser* parser, uint8_t min_bp)
{
  parser->callno++;
  Expr* left_side = parser_parse_nud(parser);
  parser_advance(parser);

  while (1)
  {
    if (parser->current.kind == TOKEN_EOF
        || parser->current.kind == TOKEN_RPAREN)
      break;

    Token op_token = parser->current;
    if (op_token.kind != TOKEN_OP)
    {
      PANIC("parser_parse_expr(): expected operator (found: %s, call: #%u)\n",
            lexer_extract_token(op_token), parser->callno);
    }

    uint8_t bps = get_binding_power(op_token.as.op);
    if (GET_LEFT_BP(bps) < min_bp) break;
    parser_advance(parser); // consume operator token

    Expr* right_side = parser_parse_expr(parser, GET_RIGHT_BP(bps));
    left_side = expr_binary(parser->arena, left_side, right_side,
                            op_token.as.op);
  }

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
        case OP_SUM:      return left + right;
        case OP_SUBTRACT: return left - right;
        case OP_MULTIPLY: return left * right;
        case OP_DIVIDE:   return left / right;
        default: UNREACHABLE("eval(): operator\n");
      }
    default: // switch (arena->items[current].kind)
      UNREACHABLE("eval(): expression kind\n");
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
    (void)parser_build_ast(&arena, buffer);
    printf("%d\n", eval_arena(&arena));
    free(arena.items);

    printf("$> ");
  }
  return 0;
}
