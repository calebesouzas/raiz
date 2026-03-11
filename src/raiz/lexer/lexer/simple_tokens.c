#include "raiz_arrays.h"
#include "tokens.h"
#include "state.h"

int handle_simple_token(LexerState* state, char*const source_code) {
  backup_start(state);

  switch (source_code[state->current]) {
  // single character tokens
  case '@': push_token(state, RAIZ_TOKEN_AT);       break;
  case '#': push_token(state, RAIZ_TOKEN_HASH);     break;
  case '$': push_token(state, RAIZ_TOKEN_DOLLAR);   break;
  case '%': push_token(state, RAIZ_TOKEN_PERCENT);  break;
  case '+': push_token(state, RAIZ_TOKEN_PLUS);     break;
  case '*': push_token(state, RAIZ_TOKEN_STAR);     break;
  case '/': push_token(state, RAIZ_TOKEN_SLASH);    break;
  case '?': push_token(state, RAIZ_TOKEN_QUESTION); break;
  case '.': push_token(state, RAIZ_TOKEN_DOT);      break;
  case ',': push_token(state, RAIZ_TOKEN_COMMA);    break;
  case ':': push_token(state, RAIZ_TOKEN_COLLON);   break;
  //// paired single character tokens
  case '(': push_token(state, RAIZ_TOKEN_OPEN_PARENTHESES);     break;
  case ')': push_token(state, RAIZ_TOKEN_CLOSE_PARENTHESES);    break;
  case '[': push_token(state, RAIZ_TOKEN_OPEN_BRACKETS);        break;
  case ']': push_token(state, RAIZ_TOKEN_CLOSE_BRACKETS);       break;
  case '{': push_token(state, RAIZ_TOKEN_OPEN_CURLY_BRACES);    break;
  case '}': push_token(state, RAIZ_TOKEN_CLOSE_CURLY_BRACES);   break;
  // two character tokens
  //// (maybe) composed token
  case '!':
    if (source_code[state->current] == '=') {
      push_token(state, RAIZ_TOKEN_NOT_EQUAL);
      state->current++;
    } else {
      push_token(state, RAIZ_TOKEN_EXCLAMATION);
    }
    break;
  //// duplicated character tokens
  case '&':
    if (source_code[state->current + 1] == '&') {
      push_token(state, RAIZ_TOKEN_DOUBLE_AMPERSAND);
      state->current++;
    } else {
      push_token(state, RAIZ_TOKEN_AMPERSAND);
    }
    break;
  case '|':
    if (source_code[state->current + 1] == '|') {
      push_token(state, RAIZ_TOKEN_DOUBLE_PIPE);
      state->current++;
    } else {
      push_token(state, RAIZ_TOKEN_PIPE);
    }
    break;
  case '^':
    if (source_code[state->current + 1] == '^') {
      push_token(state, RAIZ_TOKEN_DOUBLE_HAT);
      state->current++;
    } else {
      push_token(state, RAIZ_TOKEN_HAT);
    } 
    break;
  //// maybe duplicated or composed two character tokens
  case '=':
    switch (source_code[state->current + 1]) {
    case '=':
      push_token(state, RAIZ_TOKEN_EQUAL);
      state->current++;
      break;
    case '>':
      push_token(state, RAIZ_TOKEN_FAT_ARROW);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_ASSIGN); break;
    }
    break;
  case '-':
    switch (source_code[state->current + 1]) {
    case '>':
      push_token(state, RAIZ_TOKEN_THIN_ARROW);
      state->current++;
      break;
    case '`':
      push_token(state, RAIZ_TOKEN_HALF_THIN_ARROW);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_MINUS); break;
    }
    break;
  case '~':
    switch (source_code[state->current + 1]) {
    case '=':
      push_token(state, RAIZ_TOKEN_ALMOST_EQUAL);
      state->current++;
      break;
    case '>':
      push_token(state, RAIZ_TOKEN_WAVE_ARROW);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_TILDE); break;
    }
    break;
  case '>':
    switch (source_code[state->current + 1]) {
    case '>':
      push_token(state, RAIZ_TOKEN_DOUBLE_GREATER);
      state->current++;
      break;
    case '=':
      push_token(state, RAIZ_TOKEN_GREATER_OR_EQUAL);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_GREATER_THAN); break;
    }
    break;
  case '<':
    switch (source_code[state->current + 1]) {
    case '<':
      push_token(state, RAIZ_TOKEN_DOUBLE_LESS);
      state->current++;
      break;
    case '=':
      push_token(state, RAIZ_TOKEN_LESS_OR_EQUAL);
      state->current++;
      break;
    default: push_token(state, RAIZ_TOKEN_LESS_THAN); break;
    }
    break;
  default:
    return 0;
  }

  unsigned int len = state->current - state->start + 1;
  state->tokens[array_len(state->tokens) - 1].len = len;

  return 1;
}
