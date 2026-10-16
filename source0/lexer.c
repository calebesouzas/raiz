#include "lexer.h"

Lexer lexer_init(const char *source, size_t length)
{
  return (Lexer){
    .source = source,
    .source_length = length,
    .cursor = source,
    .remaining = length,
    .line_count = 1,
    .column_count = 1,
  };
}

void skip_whitespace(Lexer *L)
{
next:
  switch (*L->cursor)
  {
    case ' ': case '\r': case '\t':
      L->cursor++;
      L->column_count++;
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

  Token token = {0};
  token.lexeme = L->cursor;

  token.location.index = L->source_length - L->remaining;
  token.location.column = L->column_count;
  token.location.line = L->line_count;

  if (L->remaining == 0)
  {
    token.type = TOKEN_END_OF_FILE;
    return token;
  }

  switch (*L->cursor)
  {
    case '\0':
      token.type = TOKEN_END_OF_FILE;
      L->remaining = 0;
      break;
    case '\n':
      token.type = TOKEN_LINE_BREAK;
      do
      {
        L->line_count++;
        L->column_count = 0;
        token.length++;
      } while (L->cursor[token.length] == '\n'); // NOTE: it won't work with "\r\n" in sequence
      token.length = 1;
      break;
    case '+':
      token.type = TOKEN_PLUS;
      token.length++;
      break;
    case '-':
      token.type = TOKEN_MINUS;
      token.length++;
      break;
    case '*':
      token.type = TOKEN_STAR;
      token.length++;
      break;
    case '/':
      token.type = TOKEN_SLASH;
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
    case '=':
      token.type = TOKEN_EQUAL;
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
    else if (isalpha(*L->cursor))
    {
      token.type = TOKEN_IDENTIFIER;
      do
      {
        token.length++;
      } while (isalnum(L->cursor[token.length]) || L->cursor[token.length] == '_');
    }
    else
    {
      token.type = TOKEN_ERROR;
      token.as.error.message = "invalid character";
    }
    break;
  }

  L->cursor += token.length;
  L->column_count += token.length;
  L->remaining -= token.length;

  return token;
}
