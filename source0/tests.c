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
