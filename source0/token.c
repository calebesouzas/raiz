#include "token.h"

#define TOKEN_ARENA_CAPACITY (4 * 1024 * 1024)
TokenArena *new_token_arena(void)
{
  TokenArena *arena = malloc(sizeof(*arena));
  if (arena == NULL)
    return NULL;

  arena->tokens = malloc(TOKEN_ARENA_CAPACITY);
  if (arena->tokens == NULL)
  {
    free(arena);
    return NULL;
  }

  arena->count = 0;
  arena->next = NULL;

  return arena;
}

void free_token_arena(TokenArena *arena)
{
  TokenArena *current = arena;

  while (current != NULL)
  {
    free(current->tokens);

    TokenArena *previous = current;
    current = current->next;
    free(previous);
  }
}

Token *new_token(TokenArena *arena)
{
  if (arena == NULL)
    return NULL;

  TokenArena *current = arena;
  if ((current->count + 1) * sizeof(Token) > TOKEN_ARENA_CAPACITY)
  {
    current->next = new_token_arena();
    if (current->next == NULL)
      return NULL;

    current = current->next;
  }

  return &current->tokens[current->count++];
}

bool token_is_operator(Token *token)
{
  switch (token->type)
  {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
      return true;
    case TOKEN_NUMBER:
    case TOKEN_LINE_BREAK:
    case TOKEN_OPEN_PAREN:
    case TOKEN_END_OF_FILE:
    case TOKEN_CLOSE_PAREN:
      return false;
  }
}
