#include "lexer.h"
#include "report.h"

Lexer lexer_init(const char *source, size_t length, const char *file_path)
{
  return (Lexer){
    .source = source,
    .source_length = length,
    .cursor = source,
    .remaining = length,
    .line_count = 1,
    .column_count = 1,
    .file_path = file_path,
  };
}

void skip_whitespace(Lexer *L, uint32_t flags)
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
    case '\n':
      if ((flags & LEXERFLAG_ENABLE_NEW_LINE) == 0)
      {
        L->cursor++;
        L->column_count = 1;
        L->line_count++;
        L->remaining--;
        goto next;
      }
      break;
    default:
      return;
  }
}

Token next_token(Lexer *L, uint32_t flags)
{
  skip_whitespace(L, flags);

  Token token = {0};

  token.span.text = L->cursor;
  token.span.index = L->source_length - L->remaining;
  token.span.column = L->column_count;
  token.span.line = L->line_count;
  token.span.file_path = L->file_path;

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
        token.span.length++;
      } while (L->cursor[token.span.length] == '\n'); // NOTE: it won't work with "\r\n" in sequence
      token.span.length = 1;
      break;
    case '+':
      token.type = TOKEN_PLUS;
      token.span.length++;
      break;
    case '-':
      token.type = TOKEN_MINUS;
      token.span.length++;
      break;
    case '*':
      token.type = TOKEN_STAR;
      token.span.length++;
      break;
    case '/':
      token.type = TOKEN_SLASH;
      token.span.length++;
      break;
    case '(':
      token.type = TOKEN_OPEN_PAREN;
      token.span.length++;
      break;
    case ')':
      token.type = TOKEN_CLOSE_PAREN;
      token.span.length++;
      break;
    case '{':
      token.type = TOKEN_OPEN_CURLY;
      token.span.length++;
      break;
    case '}':
      token.type = TOKEN_CLOSE_CURLY;
      token.span.length++;
      break;
    case '=':
      token.type = TOKEN_EQUAL;
      token.span.length++;
      break;
    default:
    if (isdigit(*L->cursor))
    {
      token.type = TOKEN_NUMBER;
      int number = 0;
      do
      {
        number = (number * 10) + (L->cursor[token.span.length] - '0');
        token.span.length++;
      } while (isdigit(L->cursor[token.span.length]));
      token.as.literal = number;
    }
    else if (isalpha(*L->cursor))
    {
      token.type = TOKEN_IDENTIFIER;
      do
      {
        token.span.length++;
      } while (isalnum(L->cursor[token.span.length]) || L->cursor[token.span.length] == '_');
    }
    else
    {
      token.type = TOKEN_ERROR;
      token.as.error.message = "invalid character";
      report(&token.span, "error: %s\n", token.as.error.message);
    }
    break;
  }

  L->cursor += token.span.length;
  L->column_count += token.span.length;
  L->remaining -= token.span.length;

  return token;
}
