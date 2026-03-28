#ifndef RAIZ_PARSER_SOURCE
#define RAIZ_PARSER_SOURCE

#include "lexer.h"
#include "common.h"
#include "parser.h"
#include "expressions.c"

/*** Helpers definitions (declarations and info are in the top part) ***/

#define peek(p)             parser_peek(p)
#define next(p)             parser_next(p)
#define advance(p)          parser_advance(p)
#define match(p, e)         parser_match(p, e)
#define match_op(p, o)      parser_match_op(p, o)
#define match_next(p, e)    parser_match_next(p, e)
#define match_next_op(p, o) parser_match_next_op(p, o)

static inline Token parser_peek(Parser *par)
{
  return par->current;
}

static inline Token parser_next(Parser *par)
{
  return par->next;
}

static inline Token parser_advance(Parser *par)
{
  Token saved = par->current;
  par->current = par->next;
  par->next = lexer_next_token(par->lexer);
  return saved;
}

static inline bool parser_match(Parser *par, TokenKind expected)
{
  if (peek(par).kind != expected) return false;
  return true;
}

static inline bool parser_match_op(Parser *par, Operator expected)
{
  if (peek(par).kind != TOKEN_OP || par->next.as.op != expected) return false;
  return true;
}

static inline bool parser_match_next(Parser *par, TokenKind expected)
{
  return (next(par).kind == expected);
}

static inline bool parser_match_next_op(Parser *par, Operator expected)
{
  return (match_next(par, TOKEN_OP) && next(par).as.op == expected);
}

// The main expression parsing function. It uses Pratt Parsing technique
Expr *parse_expr(Parser *par, uint8_t min_bp);

/* parse_nud():
 * responsible for parsing single expressions, like literals, unary expressions
 * (only negative for now) and variables (not added yet).
 * It returns a pointer to the resulting expression */
Expr *parse_nud(Parser *par)
{
  switch (peek(par).kind)
  {
    // i should take care of 'return NULL' parts... They'll certainly cause
    // segfaults... When we call 'push_expr()'. Or maybe even before it...
    // TODO: print the error or put it somewhere
    case TOKEN_ERROR: advance(par); return parse_nud(par);
    case TOKEN_EOF: PANIC("unexpected end of file\n");
    case TOKEN_LIT_INT:
      return expr_literal(par->arena, advance(par).as.literal);
    case TOKEN_IDENT:
      TODO("parse_nud(): handle variable expressions\n");
    case TOKEN_OP:
      if (match_op(par, OP_SUBTRACT))
      {
        uint8_t bind_power = GET_RIGHT_BP(get_binding_power(peek(par).as.op));
        return expr_unary(par->arena, parse_expr(par, bind_power));
      }
      PANIC("invalid unary operator: '%.*s'\n",
          peek(par).len, peek(par).lexeme);
    case TOKEN_LPAREN:
      advance(par);
      Expr *result = parse_expr(par, 0);
      if (!match(par, TOKEN_RPAREN))
      {
        PANIC("syntax-error: expected ')' before '%.*s'\n",
            peek(par).len, peek(par).lexeme);
      }
      advance(par);
      return result;
    case TOKEN_RPAREN:
      // TODO: should i really panic on it?
      PANIC("trailing ')' before '%.*s'\n", next(par).len, next(par).lexeme);
    default:
    UNREACHABLE("parse_nud(): token kind");
  }
}

/* parse_expr():
 * responsible for parsing everything related to expressions.
 * It uses Pratt Parsing technique to mount binary expressions.
 * Returns a pointer to the resulting expression.
 * (with 'id' relative to 'par.arena.items' and is contained in there) */
Expr *parse_expr(Parser *par, uint8_t min_bp)
{
  Expr *left_side = parse_nud(par);

  while (!match(par, TOKEN_EOF) && !match(par, TOKEN_RPAREN))
  {
    if (!match(par, TOKEN_OP))
    {
      PANIC("syntax-error: expected an operator, found '%.*s'\n",
          peek(par).len, peek(par).lexeme);
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

#undef match_next_op
#undef match_next
#undef match_op
#undef match
#undef advance
#undef next
#undef peek

#endif // RAIZ_PARSER_SOURCE
