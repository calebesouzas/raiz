#include "libc.h"
#include "lexer.h"
#include "parser.h"

static inline Token *parser_current(Parser *P)
{
  return &P->buffer[1];
}

static inline Token *parser_previous(Parser *P)
{
  return &P->buffer[0];
}

static inline Token *parser_peek(Parser *P)
{
  return &P->buffer[2];
}

static inline Token *parser_advance(Parser *P)
{
  P->buffer[0] = P->buffer[1];
  P->buffer[1] = P->buffer[2];
  P->buffer[2] = next_token(&P->lexer);
  return parser_current(P);
}

static inline void check_depth(uint32_t depth)
{
  if (depth > PARSER_DEPTH_LIMIT)
  {
    fprintf(stderr, "reached parser recursion depth limit (%u)\n", PARSER_DEPTH_LIMIT);
    abort();
  }
}

Expr *parse_expr(Parser *P, uint32_t depth);

Expr *parse_nud(Parser *P, uint32_t depth)
{
  check_depth(depth);

  Token *token = parser_current(P);
  Expr *res = NULL;

again:
  switch (token->type)
  {
    case TOKEN_OPEN_PAREN:
      parser_advance(P);
      Expr *inner = parse_expr(P, depth + 1);
      if (inner == NULL)
        return NULL;

      res = new_expr_node(P->arena);
      if (res == NULL)
        return NULL;

      res->as.group.inner = inner;
      res->type = EXPR_GROUP;
      break;
    case TOKEN_PLUS:
      parser_advance(P);
      goto again;
    case TOKEN_MINUS:
      parser_advance(P);
      Token operator = *parser_previous(P);

      // weird... at this point `inner` is already declared
      inner = parse_expr(P, depth + 1);
      if (inner == NULL)
        return NULL;

      res = new_expr_node(P->arena);
      if (res == NULL)
        return NULL;

      res->type = EXPR_UNARY;
      res->as.unary.inner = inner;
      res->as.unary.operator = operator;
      break;
    case TOKEN_NUMBER:
      res = new_expr_node(P->arena);
      if (res == NULL)
        return NULL;

      res->type = EXPR_LITERAL;
      res->as.literal.value = token->as.literal;
      break;
    case TOKEN_LINE_BREAK:
    case TOKEN_END_OF_FILE:
    case TOKEN_CLOSE_PAREN:
      fprintf(stderr, "current token is not a null-denotation starter\n");
      return NULL;
  }

  return res;
}

Expr *parse_expr(Parser *P, uint32_t depth)
{
  check_depth(depth);

  Expr *left = parse_nud(P, depth + 1);
  if (left == NULL)
    return NULL;

  while (token_is_operator(parser_peek(P)))
  {
    Token operator = *parser_advance(P);
    parser_advance(P);

    Expr *right = parse_expr(P, depth + 1);
    if (right == NULL)
      return NULL;

    Expr *expr = new_expr_node(P->arena);
    if (expr == NULL)
      return NULL;

    expr->type = EXPR_BINARY;
    expr->as.binary.operator = operator;
    expr->as.binary.left = left;
    expr->as.binary.right = right;
    left = expr;
  }

  return left;
}

Ast parse(const char *source, const size_t size)
{
  Ast ast = {0};

  ast.arena = new_expr_arena();
  if (ast.arena == NULL)
    return ast;

  Parser parser = {0};
  parser.lexer.cursor = source;
  parser.lexer.remaining = size;
  parser.lexer.source = source;
  parser.lexer.size = size;

  // start from index 1 (which is the current token)
  for (int i = 1; i < sizeof(parser.buffer)/sizeof(parser.buffer[0]); i++)
  {
    parser.buffer[i] = next_token(&parser.lexer);
  }

  parser.arena = ast.arena;

  ast.root = parse_expr(&parser, 0);
  return ast;
}
