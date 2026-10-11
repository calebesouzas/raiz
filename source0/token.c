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

typedef enum {
  TOKENFLAG_OPERATOR = 1 << 0,
  TOKENFLAG_LITERAL = 1 << 1,
} TokenFlags;

static const uint32_t token_flags_map[__TOKEN_COUNT] = {
  [TOKEN_PLUS] = TOKENFLAG_OPERATOR,
  [TOKEN_MINUS] = TOKENFLAG_OPERATOR,
  [TOKEN_NUMBER] = TOKENFLAG_LITERAL,
  [TOKEN_OPEN_PAREN] = TOKENFLAG_OPERATOR,
};

bool token_is_operator(Token *token)
{
  return (token_flags_map[token->type] & TOKENFLAG_OPERATOR) != 0; // single flag, this is OK
}
