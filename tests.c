#include "source0/libc.h"
#include "source0/files.h"
#include "source0/lexer.h"

bool test_lexer(const char *source, const size_t size)
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

typedef bool (*TestFunction) (const char *source, const size_t size);

int main(void)
{
  char *source = NULL;
  size_t size = 0;
  if (!read_entire_file("test.raiz", &source, &size))
    return 1;

  const TestFunction test_functions[] = {
    test_lexer,
  };

  unsigned int total = 0;
  unsigned int success = 0;
  unsigned int failure = 0;

  for (unsigned int index = 0; index < sizeof(test_functions)/sizeof(test_functions[0]); index++)
  {
    if (test_functions[index](source, size))
    {
      success++;
    }
    else
    {
      failure++;
    }
    total++;
  }

  printf("resume: successful: %u, failed: %u, total: %u\n", success, failure, total);

  return failure > 0;
}

#include "source0/files.c"
#include "source0/lexer.c"
