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
  [TOKEN_STAR] = TOKENFLAG_OPERATOR,
  [TOKEN_SLASH] = TOKENFLAG_OPERATOR,
  [TOKEN_NUMBER] = TOKENFLAG_LITERAL,
  [TOKEN_OPEN_PAREN] = TOKENFLAG_OPERATOR,
  [TOKEN_EQUAL] = TOKENFLAG_OPERATOR,
};

bool token_is_operator(Token *token)
{
  return (token_flags_map[token->type] & TOKENFLAG_OPERATOR) != 0; // single flag, this is OK
}

const char *token_name(TokenType type)
{
  switch (type)
  {
    case TOKEN_END_OF_FILE: return "end of file";
    case TOKEN_ERROR: return "error";
    case TOKEN_NUMBER: return "number";
    case TOKEN_IDENTIFIER: return "identifier";
    case TOKEN_PLUS: return "+";
    case TOKEN_MINUS: return "-";
    case TOKEN_STAR: return "*";
    case TOKEN_SLASH: return "/";
    case TOKEN_EQUAL: return "=";
    case TOKEN_OPEN_PAREN: return "(";
    case TOKEN_CLOSE_PAREN: return ")";
    case TOKEN_LINE_BREAK: return "line break";
    case TOKEN_OPEN_CURLY: return "{";
    case TOKEN_CLOSE_CURLY: return "}";
    case __TOKEN_COUNT: return NULL;
  }
  fprintf(stderr, "token_name(): unhandled token (id %d)\n", type);
  abort();
}

typedef union {
  struct {
    uint8_t infix;
    uint8_t prefix;
  };
  uint32_t combined;
} OperatorPrecedence;

static const OperatorPrecedence operator_precedence_map[__TOKEN_COUNT] = {
  [TOKEN_EQUAL] = {.infix = 10, .prefix = 0},
  [TOKEN_PLUS] = {.infix = 20, .prefix = 1},
  [TOKEN_MINUS] = {.infix = 20, .prefix = 21},
  [TOKEN_STAR] = {.infix = 30, .prefix = 31},
  [TOKEN_SLASH] = {.infix = 30, .prefix = 0},
  [TOKEN_OPEN_PAREN] = {.infix = 0, .prefix = 1},
};

uint8_t operator_precedence(Token *token, OperatorPrecedenceType type)
{
#ifndef NO_DEBUG
  if (!token_is_operator(token))
  {
    fprintf(stderr, "operator '%.*s' is not marked as an operator\n", __TOKEN_FMT(token));
    return 0;
  }
#endif
  switch (type)
  {
    case OPERATOR_INFIX:
#ifndef NO_DEBUG
      if (operator_precedence_map[token->type].infix == 0)
      {
        fprintf(stderr, "operator '%.*s' has no infix precedence!\n", __TOKEN_FMT(token));
      }
#endif
      return operator_precedence_map[token->type].infix;
    case OPERATOR_PREFIX:
#ifndef NO_DEBUG
      if (operator_precedence_map[token->type].prefix == 0)
      {
        fprintf(stderr, "operator '%.*s' has no prefix precedence!\n", __TOKEN_FMT(token));
      }
#endif
      return operator_precedence_map[token->type].prefix;
  }
  return 0;
}
