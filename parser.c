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

static inline Rz_Token parser_peek(Rz_Parser *par)
{
  return par->current;
}

static inline Rz_Token parser_next(Rz_Parser *par)
{
  return par->next;
}

static inline Rz_Token parser_advance(Rz_Parser *par)
{
  Rz_Token saved = par->current;
  par->current = par->next;
  par->next = rz_lexer_next_token(par->lexer);
  return saved;
}

static inline bool parser_match(Rz_Parser *par, Rz_TokenKind expected)
{
  if (peek(par).kind != expected) return false;
  return true;
}

static inline bool parser_match_op(Rz_Parser *par, Rz_Operator expected)
{
  if (peek(par).kind != RZ_TOKEN_OP || par->next.as.op != expected) return false;
  return true;
}

static inline bool parser_match_next(Rz_Parser *par, Rz_TokenKind expected)
{
  return (next(par).kind == expected);
}

static inline bool parser_match_next_op(Rz_Parser *par, Rz_Operator expected)
{
  return (match_next(par, RZ_TOKEN_OP) && next(par).as.op == expected);
}

// The main expression parsing function. It uses Pratt Parsing technique
Rz_Expr *rz_parse_expr(Rz_Parser *par, uint8_t min_bp);

/* parse_nud():
 * responsible for parsing single expressions, like literals, unary expressions
 * (only negative for now) and variables (not added yet).
 * It returns a pointer to the resulting expression */
Rz_Expr *rz_parse_nud(Rz_Parser *par)
{
  switch (peek(par).kind)
  {
    // i should take care of 'return NULL' parts... They'll certainly cause
    // segfaults... When we call 'push_expr()'. Or maybe even before it...
    // TODO: print the error or put it somewhere
    case RZ_TOKEN_ERROR: 
      RZ_LOG(RZ_LOG_ERROR, "%s\n", peek(par).lexeme);
      advance(par);
      return rz_parse_nud(par);
    case RZ_TOKEN_EOF: return rz_expr_void(par->arena);
    case RZ_TOKEN_LITERAL:
      return rz_expr_literal(par->arena, advance(par).as.literal);
    case RZ_TOKEN_IDENT:
      return rz_expr_variable(par->arena, peek(par).lexeme, advance(par).len);
    case RZ_TOKEN_OP:
      if (match_op(par, RZ_OP_SUBTRACT))
      {
        uint8_t bind_power = RZ_GET_RIGHT_BP(
            rz_get_binding_power(peek(par).as.op));
        return rz_expr_unary(par->arena, rz_parse_expr(par, bind_power));
      }
      RZ_PANIC("invalid unary operator: '%.*s'\n",
          peek(par).len, peek(par).lexeme);
    case RZ_TOKEN_LPAREN:
      advance(par);
      Rz_Expr *result = rz_parse_expr(par, 0);
      if (!match(par, RZ_TOKEN_RPAREN))
      {
        RZ_PANIC("syntax-error: expected ')' before '%.*s'\n",
            peek(par).len, peek(par).lexeme);
      }
      advance(par);
      return result;
    case RZ_TOKEN_RPAREN:
      // TODO: should i really panic on it?
      RZ_PANIC("trailing ')' before '%.*s'\n", next(par).len, next(par).lexeme);
    default:
    RZ_UNREACHABLE("parse_nud(): token kind");
  }
}

/* parse_expr():
 * responsible for parsing everything related to expressions.
 * It uses Pratt Parsing technique to mount binary expressions.
 * Returns a pointer to the resulting expression.
 * (with 'id' relative to 'par.arena.items' and is contained in there) */
Rz_Expr *rz_parse_expr(Rz_Parser *par, uint8_t min_bp)
{
  Rz_Expr *left_side = rz_parse_nud(par);

  while (!match(par, RZ_TOKEN_EOF) && !match(par, RZ_TOKEN_RPAREN))
  {
    if (!match(par, RZ_TOKEN_OP))
    {
      RZ_PANIC("syntax-error: expected an operator, found '%.*s'\n",
          peek(par).len, peek(par).lexeme);
    }

    Rz_Operator op = peek(par).as.op;

    uint16_t powers = rz_get_binding_power(op);
    if (RZ_GET_LEFT_BP(powers) < min_bp) break;
    advance(par);

    Rz_Expr *right_side = rz_parse_expr(par, RZ_GET_RIGHT_BP(powers));

    left_side = rz_expr_binary(par->arena, left_side, right_side, op);
  }

  return left_side;
}

Rz_Parser rz_parser_new(Rz_Lexer *lexer, Rz_ExprArena *arena)
{
  Rz_Parser par = {0};
  par.lexer = lexer;
  par.arena = arena;
  // bootstrap 'par.next'
  advance(&par);
  // fill 'par.current' and update 'par.next'
  advance(&par);
  return par;
}

Rz_Expr *rz_parser_build_ast(Rz_ExprArena *arena, const char *source)
{
  Rz_Lexer lexer = rz_lexer_new(source);
  Rz_Parser parser = rz_parser_new(&lexer, arena);
  return rz_parse_expr(&parser, 0);
}

#undef match_next_op
#undef match_next
#undef match_op
#undef match
#undef advance
#undef next
#undef peek

#endif // RAIZ_PARSER_SOURCE
