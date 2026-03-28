#ifndef RAIZ_LEXER_SOURCE
#define RAIZ_LEXER_SOURCE

// I don't know if i need to put guards for source files...
// But why not? We're using an Unity Build system so the most we can avoid
// cycling dependencies (in this case, '#includes') the better...

#include "common.h"
#include "lexer.h"

/* Creates a lexer from nul-terminated string */
Lexer lexer_new(const char *source)
{
  return (Lexer) {
    .source = source, .current = 0, .source_len = strlen(source)
  };
}

/* Some forward declarations because the compilers are obsessed with extremely
 * high performance (which is good but makes us have more work sometimes)
 * NOTE: i'm talking about that "Single Pass Compiler" thing */

static inline Token number_literal(Lexer *lex);
static inline Token operator(Lexer *lex, Operator op);
static inline Token lexer_error(const char *message);
static inline Token identifier(Lexer *lex);

/*** Some helpers, because i don't want to get crazy of so much typing ***/

// It's really annoying that using Unity Build style - when we organize
// '#include's in a way that every header and source ends in one single compile
// unit - 'static' doesn't make functions private...
#define peek(l)     lexer_peek(l)
#define next(l)     lexer_next(l)
#define prev(l)     lexer_prev(l)
#define match(l, e) lexer_match(l, e)
#define advance(l)  lexer_advance(l)
#define error(m)    lexer_error(m)

/* just returns the current character */
static inline char lexer_peek(Lexer *lex)
{
  return lex->source[lex->current];
}

/* returns the next character if there is one */
static inline char lexer_next(Lexer *lex)
{
  if (lex->current + 1 >= lex->source_len) return '\0';
  return lex->source[lex->current + 1];
}

static inline char lexer_prev(Lexer *lex)
{
  if (lex->current - 1 >= lex->source_len) return '\0';
  return lex->source[lex->current - 1];
}

/* returns the current character after advancing the position */
static inline char lexer_advance(Lexer *lex)
{
  return lex->source[lex->current++];
}

/* returns 'true' if the current character is equal to the expected one
 * and updates the lexer's position if it is, else, returns 'false' */
static inline bool lexer_match(Lexer *lex, char expected)
{
  if (peek(lex) != expected) return false;
  lex->current++;
  return true;
}

/* skip_whitespace() will skip unrelevant characters */
/* NOTE: as Raiz gets developed, we need to take care about '\n' (line feed)
 * character, since this is what will be used to mark end of "statements" */
static inline void skip_whitespace(Lexer *lex)
{
  // For some reason 'isspace(c)' doesn't return 'true' if 'c' is ' '...
  // It is not 'isspace'ing... Yeah, that was a terrible joke.
  while (isspace(peek(lex)) || peek(lex) == ' ') advance(lex);
}

/* returns a token with basic information, intended for simple kind of tokens */
static inline Token make_token(Lexer *lex, TokenKind kind)
{
  return (Token) {
    .kind = kind,
    .lexeme = lex->source + lex->start,
    .len = lex->current - lex->start,
  };
}

/* main tokenization process function */
/* It will skip white spaces, return a token and also update the lexer state
 * by updating 'start' and 'current' fields, in case of failure it returns an
 * error token with a message */
Token lexer_next_token(Lexer *lex)
{
  skip_whitespace(lex);

  lex->start = lex->current;

  char c = advance(lex);

  if (isdigit(c)) return number_literal(lex);
  else if (isalpha(c)) return identifier(lex);

  // Sorry for the nested ternaries...
  switch (c)
  {
    case '+': return operator(lex, OP_SUM);
    case '-': return operator(lex, OP_SUBTRACT);
    case '*': return operator(lex, OP_MULTIPLY);
    case '/': return operator(lex, OP_DIVIDE);
    case '%': return operator(lex, OP_MODULE);
    case '=': return match(lex, '=') ? operator(lex, OP_EQUAL)
                                     : operator(lex, OP_ASSIGN);
    case '!': return match(lex, '=') ? operator(lex, OP_NOT_EQUAL)
                                     : operator(lex, OP_BANG);
    case '|': return match(lex, '|') ? operator(lex, OP_BOOL_OR)
                                     : operator(lex, OP_BIT_OR);
    case '&': return match(lex, '&') ? operator(lex, OP_BOOL_AND)
                                     : operator(lex, OP_BIT_AND);
    case '>': return match(lex, '>') ? operator(lex, OP_BIT_RSHIFT)
                                     : match(lex, '=')
                                       ? operator(lex, OP_GREATER_EQ)
                                       : operator(lex, OP_GREATER);
    case '<': return match(lex, '<') ? operator(lex, OP_BIT_LSHIFT)
                                     : match(lex, '=')
                                       ? operator(lex, OP_LESS_EQ)
                                       : operator(lex, OP_LESS);
    default: // Do i really need to indent after it?
    if (peek(lex) == '\0') return make_token(lex, TOKEN_EOF);
    else
    {
      char buffer[1024] = {0};
      snprintf(
          buffer, sizeof(buffer),
          "unexpected character '%c' (byte %x)", // is 'x' a real format spec?
          c, c
      );
      // need to strdup(), unless, we'd read garbage. If it doesn't segfault
      return error(strdup(buffer));
    }
  }
}

static inline Token number_literal(Lexer *lex)
{
  int number = 0;
  lex->current--; // this will prevent us from skipping the first digit

  // TODO: learn how to do the reverse operation (extract string from a number)
  while (isdigit(peek(lex))) number = (number * 10) + (advance(lex) - '0');

  Token token = make_token(lex, TOKEN_LIT_INT);
  token.as.literal = number;
  return token;
}

static inline Token identifier(Lexer *lex)
{
  while (isalnum(peek(lex))) advance(lex);

  // TODO: check for keywords when Raiz is finally ready for them
  Token token = make_token(lex, TOKEN_IDENT);
  return token;
}

static inline Token operator(Lexer *lex, Operator op)
{
  Token token = make_token(lex, TOKEN_OP);
  token.as.op = op;
  return token;
}

static inline Token error(const char *message)
{
  return (Token) {
    .kind = TOKEN_ERROR,
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
