#include "libc.h"
#include "assertions.h"
#include "lexer.h"
#include "parser.h"
#include "report.h"

static Token *parser_save_token(Parser *P, uint32_t flags)
{
  Token *token = new_token(P->tokens);
  if (token == NULL)
  {
    fprintf(stderr, "parser_save_token(): failed to allocate token arena\n");
    abort();
  }

  *token = next_token(&P->lexer, flags);
  return token;
}

static inline void parser_error(Parser *P, Token *token, const char *message)
{
  ParserError error = {0};
  error.token = token;
  strncpy(error.message, message, sizeof(error.message) - 1); // - 1 for the null byte
  array_push(&P->errors, error);
}

static inline Token *parser_current(Parser *P)
{
  return P->buffer[1];
}

static inline Token *parser_previous(Parser *P)
{
  return P->buffer[0];
}

static inline Token *parser_advance(Parser *P)
{
  P->buffer[0] = P->buffer[1];
  P->buffer[1] = parser_save_token(P, LEXER_DEFAULT_FLAGS);
  return parser_current(P);
}

static inline Token *parser_advance_with(Parser *P, uint32_t flags)
{
  P->buffer[0] = P->buffer[1];
  P->buffer[1] = parser_save_token(P, flags);
  return parser_current(P);
}

static inline Token *parser_next(Parser *P)
{
  parser_advance(P);
  return parser_previous(P);
}

bool parser_expect(Parser *P, TokenType expected, Token *token)
{
  if (token->type == expected)
    return true;

  char formatted_message[PARSER_MESSAGE_SIZE];
  snprintf(formatted_message, sizeof(formatted_message), "expected '%s'", token_name(expected));

  parser_error(P, token, formatted_message);
  return false;
}

static inline bool parser_consume(Parser *P, TokenType expected)
{
  if (!parser_expect(P, expected, parser_current(P)))
    return false;

  parser_advance(P);
  return true;
}

static inline Expr *parser_new_node(Parser *P)
{
  size_t id = P->pool->count;
  array_push(P->pool, (Expr){0});
  Expr *node = array_last(P->pool);
  node->id = id;
  node->span = parser_current(P)->span;
  return node;
}

static inline bool check_depth(Parser *P, uint32_t depth)
{
  if (depth > PARSER_DEPTH_LIMIT)
  {
    parser_error(P, parser_current(P), "depth limit overflow");
    return false;
  }
  return true;
}

Expr *parse_expr_tree(Parser *P, uint32_t depth, uint8_t min_precedence);
Expr *parse_expr(Parser *P, uint32_t depth);

Expr *parse_nud(Parser *P, uint32_t depth)
{
  if (!check_depth(P, depth))
    return NULL;

  Expr *res = parser_new_node(P);

again:
  switch (parser_current(P)->type)
  {
    case TOKEN_OPEN_PAREN:
      parser_advance(P);
      uint8_t precedence = operator_precedence(parser_previous(P), OPERATOR_PREFIX);
      Expr *inner = parse_expr_tree(P, depth + 1, precedence);
      if (inner == NULL)
        return NULL;

      if (!parser_consume(P, TOKEN_CLOSE_PAREN))
        return NULL;

      res->as.group.inner_id = inner->id;
      res->type = EXPR_GROUP;
      break;
    case TOKEN_OPEN_CURLY:
      parser_advance_with(P, LEXER_DEFAULT_FLAGS & ~LEXERFLAG_ENABLE_NEW_LINE);

      res->type = EXPR_BLOCK;

      while (parser_current(P)->type != TOKEN_CLOSE_CURLY)
      {
        Expr *node = parse_expr_tree(P, depth + 1, 1);
        if (node == NULL)
        {
          while (parser_current(P)->type != TOKEN_LINE_BREAK)
          {
            parser_advance(P);
            if (parser_current(P)->type == TOKEN_END_OF_FILE)
            {
              parser_error(P, parser_current(P), "unexpected end of file");
              return NULL;
            }
          }
          parser_consume(P, TOKEN_LINE_BREAK);
          continue;
        }

        if (!parser_consume(P, TOKEN_LINE_BREAK))
        {
          return NULL;
        }

        array_push(&res->as.block, node->id);
      }
      break;
    case TOKEN_PLUS:
      parser_advance(P);
      goto again;
    case TOKEN_MINUS:
      // NOTE: could remove advance() and previous() and use just next() but that would trigger a
      // compiler extension warning, so we're not doing that.
      parser_advance(P);
      Token *operator = parser_previous(P);

      precedence = operator_precedence(operator, OPERATOR_PREFIX);
      inner = parse_expr_tree(P, depth + 1, precedence);
      if (inner == NULL)
        return NULL;

      res->type = EXPR_UNARY;
      res->as.unary.inner_id = inner->id;
      res->as.unary.operator = operator;
      break;
    case TOKEN_STAR:
    case TOKEN_SLASH:
      parser_error(P, parser_current(P), "not an unary operator");
      return NULL;
    case TOKEN_NUMBER:
      res->type = EXPR_LITERAL;
      res->as.literal.value = parser_current(P)->as.literal;
      parser_advance(P);
      break;
    case TOKEN_IDENTIFIER:
    {
      Token *identifier = parser_next(P);

      res->type = EXPR_IDENTIFIER;
      res->as.identifier = (Expr_Identifier) identifier;

      if (parser_current(P)->type == TOKEN_COLLON_EQUAL)
      {
        parser_advance(P);

        res->type = EXPR_DECLARATION;
        res->as.declaration.identifier = identifier;

        Expr *value = parse_expr(P, depth + 1);
        if (value == NULL)
          return NULL;
        res->as.declaration.value_id = value->id;
      }
    } break;
    case __TOKEN_COUNT:
      fprintf(stderr, "unreachable token: __TOKEN_COUNT\n");
      abort();
    default:
      parser_error(P, parser_current(P), "not a null-denotation starter");
      return NULL;
  }

  return res;
}

Expr *parse_expr_tree(Parser *P, uint32_t depth, uint8_t min_precedence)
{
  check_depth(P, depth);

  Expr *left = parse_nud(P, depth + 1);
  if (left == NULL)
    return NULL;

  if (!token_is_operator(parser_current(P)))
  {
    if (token_is_expression_finisher(parser_current(P)))
    {
      return left;
    }
    else
    {
      parser_error(P, parser_current(P), "expected operator");
      return NULL;
    }
  }

  do
  {
    Token *operator = parser_current(P);

    uint8_t precedence = operator_precedence(operator, OPERATOR_INFIX);
    if (precedence < min_precedence)
      break;

    // NOTE: we advance after comparing the precedences!
    // If we don't and this loop breaks, the operator will be skipped and the other part of the
    // binary expression will be not resolved correctly and maybe it would even propagate further.
    parser_advance_with(P, LEXER_DEFAULT_FLAGS & ~LEXERFLAG_ENABLE_NEW_LINE);
    Expr *right = parse_expr_tree(P, depth + 1, precedence);
    if (right == NULL)
      return NULL;

    Expr *expr = parser_new_node(P);

    expr->type = EXPR_BINARY;
    expr->as.binary.operator = operator;
    expr->as.binary.left_id = left->id;
    expr->as.binary.right_id = right->id;
    left = expr;
  } while (token_is_operator(parser_current(P)));

  return left;
}

Expr *parse_expr(Parser *P, uint32_t depth)
{
  if (!check_depth(P, depth))
    return NULL;

  Expr *res = NULL;
  switch (parser_current(P)->type)
  {
    case TOKEN_ERROR:
      parser_error(P, parser_current(P), parser_current(P)->as.error.message);
      break;
    default:
      res = parse_expr_tree(P, depth + 1, 1);
      break;
  }

  return res;
}

Ast parse(const char *source, const size_t length, const char *file_path)
{
  Ast ast = {0};
  ast.ok = true;

  Parser parser = {0};
  parser.lexer = lexer_init(source, length, file_path);

  parser.tokens = new_token_arena();
  if (parser.tokens == NULL)
  {
    fprintf(stderr, "parse(): failed to allocate token arena\n");
    ast.ok = false;
    return ast;
  }

  // start from index 1 (which is the current token)
  for (size_t i = 1; i < sizeof(parser.buffer)/sizeof(parser.buffer[0]); i++)
  {
    parser.buffer[i] = parser_save_token(&parser, LEXER_DEFAULT_FLAGS);
  }

  parser.pool = &ast.pool;

  ast.root = parse_expr(&parser, 0);
  if (parser.errors.count != 0)
  {
    ast.ok = false;
    array_for(ParserError, error, &parser.errors)
    {
      Span *span = &error->token->span;
      report(span, "error: %s\n", error->message);
    }
  }
  return ast;
}
