#include "tokenizer.h"
#include "tokens.h"

#include "../helpers/switch.h"

#include "../arrays.h"
#include <stdlib.h>

#define is_next(character) (source_code[i + 1] == character)

#define is_number(c) (c >= '0' && c <= '9')
#define is_alpha(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

#define is_identstart(c) (is_alpha(c) || c == '_' )
#define is_identchar(c) (is_identstart(c) || is_number(c))

inline Token set_simple_token(TokenKind kind, uint len, uint start) {
  return (Token) {
    .kind = kind,
    .len = len,
    .start = start,
  };
}

int handle_simple_token(char*const slice, uint start_index, Token* token) {
  switch (slice[0]) {
    // TODO: handle one, two or three symbol sequence tokens
      CASE('!', 
        is_next('=') {
          setn(TOKEN_NOTEQ, 2)
        } else {
          set(TOKEN_EXCLAM)
        })
      CASE('@', set(TOKEN_AT))
      CASE('#', set(TOKEN_HASH))
      CASE('$', set(TOKEN_DOLLAR))
      CASE('%', set(TOKEN_PERCENT))
      CASE('^',
        is_next('^') {
          setn(TOKEN_OR, 2)
        } else {
          set(TOKEN_HAT)
        })
      CASE('&',
        is_next('&') {
          setn(TOKEN_AND, 2)
        } else {
          set(TOKEN_AMPER)
        })
      CASE('*', set(TOKEN_ASTERISK))
      CASE('(', set(TOKEN_LPAREN))
      CASE(')', set(TOKEN_RPAREN))
      CASE('[', set(TOKEN_LSQUARE))
      CASE(']', set(TOKEN_RSQUARE))
      CASE('{', set(TOKEN_LBRACE))
      CASE('}', set(TOKEN_RBRACE))
      CASE('-', 
        is_next('>') {
          setn(TOKEN_THIN_ARR, 2)
        } else {
          set(TOKEN_MINUS)
        })
      CASE('+', set(TOKEN_PLUS))
      CASE('=',
        is_next('=') {
          setn(TOKEN_EQUAL, 2)
        } else is_next('>') {
          setn(TOKEN_FAT_ARR, 2)
        } else {
          set(TOKEN_ASSIGN)
        })
      CASE('`', set(TOKEN_GRAVE))
      CASE(',', set(TOKEN_COMMA))
      CASE('.', set(TOKEN_DOT))
      CASE(':', set(TOKEN_COLLON))
      CASE('/', set(TOKEN_SLASH))
      CASE('?', set(TOKEN_QUESTION))
      CASE('~',
        is_next('>') {
          setn(TOKEN_WAVE_ARR, 2)
        } else {
          set(TOKEN_TILDE)
        })
      CASE('>',
        is_next('=') {
          setn(TOKEN_GT_EQ, 2)
        } else is_next('>') {
          setn(TOKEN_RSHIFT, 2)
        } else {
          set(TOKEN_GREATER)
        })
      CASE('<',
        is_next('<') {
          setn(TOKEN_LSHIFT, 2)
        } else is_next('=') {
          setn(TOKEN_LT_EQ, 2)
        } else {
          set(TOKEN_LESSER)
        })
      CASE('|',
        is_next('|') {
          setn(TOKEN_OR, 2)
        } else {
          set(TOKEN_PIPE)
        })
      CASE('\\', set(TOKEN_BACKSLASH))
      default:
  }
}

Token* raiz_tokenize(char* const source_code) {
  Token* tokens = NULL;
  Token token;

  uint line = 1;
  uint column = 0;

  for (uint i = 0; source_code[i]; ++i) {
    short add = 1;
    column++;
    switch (source_code[i]) {
    case '\n':
      line++;
      column = 0;
    case ' ':
    case '\t':
    case '\r':
      add = 0;
      break;
    default:
      if (is_number(source_code[i])) {
        // TODO: handle numbers
        int number = 0;
        uint start = i;
        for (; is_number(source_code[i]); ++i) {
          number = (number * 10) + (source_code[i] - '0');
        }
        setn(TOKEN_INT, i - start)
        token.value = (int*)malloc(sizeof(int));
        if (token.value) (*((int*)(token).value)) = number;
      } else if (is_identstart(c)) {
        uint start = i;
        // get identifier length
        for (; is_identchar(source_code[i]); ++i);
        // TODO: compare slice with each keyword
        // NOTE: i could implement a hash map for performance
        // and DX reasons
      } else {
        fprintf(
          stderr,
          "raiz: invalid character at line %u column %u: %c\n",
          line,
          column + 1,
          source_code[i]
        );
        array_free(tokens);
        return NULL;
      }
    } // switch (source_code[i]) // current character
    if (add) array_push(tokens, token);

  } // for (... source_code ...) // main loop

  return tokens;
}
