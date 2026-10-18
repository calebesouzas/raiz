#pragma once

#include "libc.h"
#include "generics.h"
#include "ast.h"
#include "lexer.h"

#define PARSER_MESSAGE_SIZE 256
typedef struct {
  const char *message;
  Token *token;
} ParserError;

typedef array_of(ParserError) ParserError_Array;

typedef struct {
  ExprArena *arena;
  Lexer lexer;
  TokenArena *tokens;
  Token *buffer[2];
  ParserError_Array errors;
} Parser;

// all parser functions should check the depth to prevent stack overflow due to indirect recursion
#define PARSER_DEPTH_LIMIT 256

Ast parse(const char *source, const size_t length);
