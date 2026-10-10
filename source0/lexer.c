#include "lexer.h"

void skip_whitespace(Lexer *L)
{
next:
  switch (*L->cursor)
  {
    case ' ': case '\r': case '\t':
      L->cursor++;
      L->remaining--;
      goto next;
    break;
    default:
    return;
  }
}

Token next_token(Lexer *L)
{
  skip_whitespace(L);

  if (L->remaining == 0)
    return (Token){TOKEN_END_OF_FILE, 0, NULL};

  Token token = {0};
  token.lexeme = L->cursor;

  switch (*L->cursor)
  {
    case '\0':
      token.type = TOKEN_END_OF_FILE;
      L->remaining = 0;
    break;
    case '\n':
      token.type = TOKEN_LINE_BREAK;
      token.length++;
      token.lexeme = NULL;
    break;
    case '+':
      token.type = TOKEN_PLUS;
      token.length++;
    break;
    case '-':
      token.type = TOKEN_MINUS;
      token.length++;
    break;
    case '(':
      token.type = TOKEN_OPEN_PAREN;
      token.length++;
    break;
    case ')':
      token.type = TOKEN_CLOSE_PAREN;
      token.length++;
    break;
    default:
    if (isdigit(*L->cursor))
    {
      token.type = TOKEN_NUMBER;
      int number = 0;
      do
      {
        number = (number * 10) + (L->cursor[token.length] - '0');
        token.length++;
      } while (isdigit(L->cursor[token.length]));
      token.as.literal = number;
    }
    else
    {
      fprintf(stderr, "%s(): invalid character: '%c'\n", __FUNCTION__, *L->cursor);
      exit(1);
    }
    break;
  }

  L->cursor += token.length;
  L->remaining -= token.length;

  return token;
}
