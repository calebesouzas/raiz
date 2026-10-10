#include "token.h"

bool token_is_operator(Token *token)
{
  switch (token->type)
  {
    case TOKEN_PLUS:
      return true;
    case TOKEN_NUMBER:
    case TOKEN_LINE_BREAK:
    case TOKEN_OPEN_PAREN:
    case TOKEN_END_OF_FILE:
    case TOKEN_CLOSE_PAREN:
      return false;
  }
}
