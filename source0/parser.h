#pragma once

#include "libc.h"
#include "ast.h"
#include "lexer.h"

typedef struct {
  ExprArena *arena;
  Lexer lexer;
  TokenArena *tokens;
  Token *buffer[3];
} Parser;

// all parser functions should check the depth to prevent stack overflow due to indirect recursion
#define PARSER_DEPTH_LIMIT 256

Ast parse(const char *source, const size_t length);
