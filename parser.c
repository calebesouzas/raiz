#ifndef RAIZ_PARSER_SOURCE
#define RAIZ_PARSER_SOURCE

#include "lexer.c"
#include "common.h"
#include "parser.h"
#include "expressions.c"

// The main expression parsing function. It uses Pratt Parsing technique
Expr *parse_expr(Parser *par, uint8_t min_bp);

/* parse_nud():
 * responsible for parsing single expressions, like literals, unary expressions
 * (only negative for now) and variables (not added yet).
 * It returns a pointer to the resulting expression */
Expr *parse_nud(Parser *par)
{
  if (match(par, TOKEN_OP) && match_op(par, OP_SUBTRACT))
  {
    uint8_t bind_power = GET_RIGHT_BP(get_binding_power(peek(par).as.op));
    return expr_unary(par->arena, parse_expr(par, bind_power));
  }
  else if (match(par, TOKEN_LPAREN))
  {
    advance(par);
    Expr *result = parse_expr(par, 0);
    if (!match(par, TOKEN_RPAREN))
    {
      // TODO: handle "expected ')'" error
    }
    advance(par);
    return result;
  }

  // TODO: handle variable expressions

  int lit = peek(par).as.literal;
  advance(par);
  return expr_literal(par->arena, lit);
}

/* parse_expr():
 * responsible for parsing everything related to expressions.
 * It uses Pratt Parsing technique to mount binary expressions.
 * Returns a pointer to the resulting expression.
 * (with 'id' relative to 'par.arena.items' and is contained in there) */
Expr *parse_expr(Parser *par, uint8_t min_bp)
{
  Expr *left_side = parse_nud(par);

  while (!match_next(par, TOKEN_EOF) && !match(par, TOKEN_RPAREN))
  {
    if (!match(par, TOKEN_OP))
    {
      // TODO: handle "expected operator" error
      return NULL; // it will cause segfaults!
      break;
    }

    Operator op = peek(par).as.op;

    uint16_t powers = get_binding_power(op);
    if (GET_LEFT_BP(powers) < min_bp) break;
    advance(par);

    Expr *right_side = parse_expr(par, GET_RIGHT_BP(powers));

    left_side = expr_binary(par->arena, left_side, right_side, op);
  }

  return left_side;
}

Parser parser_new(Lexer *lexer, ExprArena *arena)
{
  Parser par = {0};
  par.lexer = lexer;
  par.arena = arena;
  // bootstrap 'par.next'
  advance(&par);
  // fill 'par.current' and update 'par.next'
  advance(&par);
  return par;
}

Expr *parser_build_ast(ExprArena *arena, const char *source)
{
  Lexer lexer = lexer_new(source);
  Parser parser = parser_new(&lexer, arena);
  return parse_expr(&parser, 0);
}

/*** Helpers definitions (declarations and info are in the top part) ***/
static inline bool match_next_op(Parser *par, Operator expected)
{
  return (match_next(par, TOKEN_OP) && next(par).as.op == expected);
}

static inline bool match_next(Parser *par, TokenKind expected)
{
  return (next(par).kind == expected);
}

static inline bool match_op(Parser *par, Operator expected)
{
  if (peek(par).kind != TOKEN_OP || par->next.as.op != expected) return false;
  return true;
}

static inline bool match(Parser *par, TokenKind expected)
{
  if (peek(par).kind != expected) return false;
  return true;
}

static inline Token advance(Parser *par)
{
  par->current = par->next;
  par->next = lexer_next_token(par->lexer);
  return par->current;
}

static inline Token next(Parser *par)
{
  return par->next;
}

static inline Token peek(Parser *par)
{
  return par->current;
}

#endif // RAIZ_PARSER_SOURCE
