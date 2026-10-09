#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

bool read_entire_file(const char *file_path, char **p_buffer, size_t *p_size)
{
  bool result = true;

  FILE *file = fopen(file_path, "r");
  if (file == NULL)
  {
    fprintf(stderr, "%s(): failed to open '%s': %s\n", __FUNCTION__, file_path, strerror(errno));
    return false;
  }

  fseek(file, 0, SEEK_END);
  *p_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *p_buffer = malloc(*p_size);
  if (*p_buffer == NULL)
  {
    fprintf(stderr, "%s(): failed to allocate %zu bytes\n", __FUNCTION__, *p_size);
    result = false;
    goto close_file;
  }

  size_t bytes_read = fread(*p_buffer, sizeof(char), *p_size, file);
  if (bytes_read != *p_size)
  {
    fprintf(
        stderr, "%s(): failed to read %zu bytes, could read only %zu\n",
        __FUNCTION__, *p_size, bytes_read
    );
    result = false;
  }

close_file:
  fclose(file);

  return result;
}

bool process_program(const char *source, size_t size);

int main(int argc, char **argv)
{
  if (argc <= 1)
  {
    fprintf(stderr, "usage: %s <raiz file>", argv[0]);
    return 1;
  }

  char *source = NULL;
  size_t size = 0;
  if (!read_entire_file(argv[1], &source, &size))
    return 1;

  if (!process_program(source, size))
    return 1;

  free(source);
  source = NULL;

  return 0;
}

typedef struct Lexer {
  const char *source;
  size_t size;

  const char *cursor;
  size_t remaining;
} Lexer;

typedef enum TokenType {
  TOKEN_END_OF_FILE,
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_LINE_BREAK,
} TokenType;

typedef struct Token {
  TokenType type;
  size_t length;
  const char *lexeme;
  union {
    int literal;
  } as;
} Token;

Token next_token(Lexer *L)
{
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

bool process_program(const char *source, const size_t size)
{
  Lexer lexer = {0};
  lexer.source = source;
  lexer.size = size;
  lexer.cursor = source;
  lexer.remaining = size;

  Token token = {0};
  printf("[\n");
  do
  {
    token = next_token(&lexer);
    printf("  (%d)", token.type);
    if (token.lexeme != NULL)
    {
      printf(" \"%.*s\"",
          token.length > INT_MAX ? INT_MAX : (int)token.length,
          token.lexeme
      );
    }
    if (token.type == TOKEN_NUMBER)
    {
      printf(" %d", token.as.literal);
    }
    printf("\n");
  } while (token.type != TOKEN_END_OF_FILE);
  printf("]\n");

  return true;
}
