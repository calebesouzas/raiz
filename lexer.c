#ifndef RAIZ_LEXER_SOURCE
#define RAIZ_LEXER_SOURCE

// I don't know if i need to put guards for source files...
// But why not? We're using an Unity Build system so the most we can avoid
// cycling dependencies (in this case, '#includes') the better...

#include "common.h"
#include "ast.h"
#include "lexer.h"

/* Creates a lexer from nul-terminated string */
Rz_Lexer rz_lexer_new(const char *source)
{
  return (Rz_Lexer) {
    .source = source, .current = 0, .source_len = strlen(source)
  };
}

/* Some forward declarations because the compilers are obsessed with extremely
 * high performance (which is good but makes us have more work sometimes)
 * NOTE: i'm talking about that "Single Pass Compiler" thing */

static inline Rz_Token number_literal(Rz_Lexer *lex);
static inline Rz_Token operator(Rz_Lexer *lex, Rz_Operator op);
static inline Rz_Token error(const char *message);
static inline Rz_Token identifier(Rz_Lexer *lex);

/*** Some helpers, because i don't want to get crazy of so much typing ***/

/* just returns the current character */
static inline char peek(Rz_Lexer *lex)
{
  return lex->source[lex->current];
}

/* returns the next character if there is one */
static inline char next(Rz_Lexer *lex)
{
  if (lex->current + 1 >= lex->source_len) return '\0';
  return lex->source[lex->current + 1];
}

static inline char prev(Rz_Lexer *lex)
{
  if (lex->current - 1 >= lex->source_len) return '\0';
  return lex->source[lex->current - 1];
}

/* returns the current character after advancing the position */
static inline char advance(Rz_Lexer *lex)
{
  return lex->source[lex->current++];
}

/* returns 'true' if the current character is equal to the expected one
 * and updates the lexer's position if it is, else, returns 'false' */
static inline bool match(Rz_Lexer *lex, char expected)
{
  if (peek(lex) != expected) return false;
  lex->current++;
  return true;
}

/* skip_whitespace() will skip unrelevant characters */
/* NOTE: as Raiz gets developed, we need to take care about '\n' (line feed)
 * character, since this is what will be used to mark end of "statements" */
static inline void skip_whitespace(Rz_Lexer *lex)
{
  // For some reason 'isspace(c)' doesn't return 'true' if 'c' is ' '...
  // It is not 'isspace'ing... Yeah, that was a terrible joke.
  while (isspace(peek(lex)) || peek(lex) == ' ') advance(lex);
}

/* returns a token with basic information, intended for simple kind of tokens */
static inline Rz_Token make_token(Rz_Lexer *lex, Rz_TokenKind kind)
{
  return (Rz_Token) {
    .kind = kind,
    .lexeme = lex->source + lex->start,
    .len = lex->current - lex->start,
  };
}

/* main tokenization process function */
/* It will skip white spaces, return a token and also update the lexer state
 * by updating 'start' and 'current' fields, in case of failure it returns an
 * error token with a message */
Rz_Token rz_lexer_next_token(Rz_Lexer *lex)
{
  skip_whitespace(lex);

  lex->start = lex->current;

  char c = advance(lex);

  if (isdigit(c)) return number_literal(lex);
  else if (isalpha(c)) return identifier(lex);

  // Sorry for the nested ternaries...
  switch (c)
  {
    case '\0':return make_token(lex, RZ_TOKEN_EOF);
    case '\e':return make_token(lex, RZ_TOKEN_EOF); // '\e' is literally EOF
    case '(': return make_token(lex, RZ_TOKEN_LPAREN);
    case ')': return make_token(lex, RZ_TOKEN_RPAREN);
    case '+': return operator(lex, RZ_OP_SUM);
    case '-': return operator(lex, RZ_OP_SUBTRACT);
    case '*': return operator(lex, RZ_OP_MULTIPLY);
    case '/': return operator(lex, RZ_OP_DIVIDE);
    case '%': return operator(lex, RZ_OP_MODULE);
    case '^': return operator(lex, RZ_OP_BIT_XOR);
    case '=': return match(lex, '=') ? operator(lex, RZ_OP_EQUAL)
                                     : operator(lex, RZ_OP_ASSIGN);
    case '!': return match(lex, '=') ? operator(lex, RZ_OP_NOT_EQUAL)
                                     : operator(lex, RZ_OP_BANG);
    case '|': return match(lex, '|') ? operator(lex, RZ_OP_BOOL_OR)
                                     : operator(lex, RZ_OP_BIT_OR);
    case '&': return match(lex, '&') ? operator(lex, RZ_OP_BOOL_AND)
                                     : operator(lex, RZ_OP_BIT_AND);
    case '>': return match(lex, '>') ? operator(lex, RZ_OP_BIT_RSHIFT)
                                     : match(lex, '=')
                                       ? operator(lex, RZ_OP_GREATER_EQ)
                                       : operator(lex, RZ_OP_GREATER);
    case '<': return match(lex, '<') ? operator(lex, RZ_OP_BIT_LSHIFT)
                                     : match(lex, '=')
                                       ? operator(lex, RZ_OP_LESS_EQ)
                                       : operator(lex, RZ_OP_LESS);
    default: // Do i really need to indent after it?
    {
      char buffer[1024] = {0};
      snprintf(buffer, sizeof(buffer),
          "unexpected character '%c' (byte 0x%x)", c, c);
      // need to strdup(), unless, we'd read garbage. If it doesn't segfault
      return error(strdup(buffer));
    }
  }
}

static inline Rz_Token number_literal(Rz_Lexer *lex)
{
  int number = 0;
  lex->current--; // this will prevent us from skipping the first digit

  // TODO: learn how to do the reverse operation (extract string from a number)
  while (isdigit(peek(lex))) number = (number * 10) + (advance(lex) - '0');

  Rz_Token token = make_token(lex, RZ_TOKEN_LIT_INT);
  token.as.literal = number;
  return token;
}

static inline Rz_Token identifier(Rz_Lexer *lex)
{
  while (isalnum(peek(lex))) advance(lex);

  // TODO: check for keywords when Raiz is finally ready for them
  Rz_Token token = make_token(lex, RZ_TOKEN_IDENT);
  return token;
}

static inline Rz_Token operator(Rz_Lexer *lex, Rz_Operator op)
{
  Rz_Token token = make_token(lex, RZ_TOKEN_OP);
  token.as.op = op;
  return token;
}

static inline Rz_Token error(const char *message)
{
  return (Rz_Token) {
    .kind = RZ_TOKEN_ERROR,
    .lexeme = message,
    .len = strlen(message)
  };
}

#undef peek
#undef next
#undef prev
#undef advance
#undef match
#undef error

#endif // RAIZ_LEXER_SOURCE
