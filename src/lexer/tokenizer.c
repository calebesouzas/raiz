#include "tokenizer.h"
#include "tokens.h"

#include "../helpers/switch.h"

#include "../arrays.h"
#include <stdlib.h>

#define set(VARIANT) token = simple_token(VARIANT, 1, pos);
#define setn(VARIANT, NUM)\
  token = simple_token(VARIANT, NUM, pos);\
  pos.column += NUM - 1;

#define is_next(character) if (source_code[i + 1] == character)

#define is_number(c) (c >= '0' && c <= '9')
#define is_identchar(c) ((c >= '0' && c <= '9') \
			 || (c >= 'A' && c <= 'Z')\
			 || (c == '_'))

Token simple_token(TokenKind kind, unsigned int len, Position pos) {
  return (Token) {
    .kind = kind,
    .len = len,
    .pos = pos,
    .value = NULL
  };
}

inline int doubled(char c, char* const source, unsigned int start) {
  return (*(source + start + 1) == c);
}

Token* raiz_tokenize(char* const source_code) {
  Token* tokens = NULL;
  Token token;

  Position pos = (Position) {
    .column = 0,
    .line = 1
  };

  char c = '\0';
  for (unsigned int i = 0; (c = source_code[i]); ++i) {
    int add = 1;
    pos.column++;
    switch (c) {
      case '\n':
        pos.line++;
        pos.column = 0;
      case ' ':
      case '\t':
      case '\r':
        add = 0;
        break;
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
	if (is_number(source_code[i])) {
	  // TODO: handle numbers
	  int number = 0;
	  unsigned int start = i;
	  for (; is_number(source_code[i]); ++i) {
	    number = (number * 10) + (source_code[i] - '0');
	  }
	  setn(TOKEN_INT, i - start)
	  token.value = (int*)malloc(sizeof(int));
	  if (token.value) (*((int*)(token).value)) = number;
	} else {
          fprintf(
	    stderr,
	    "raiz: invalid character at line %u column %u: %c\n",
	    pos.line,
	    pos.column + 1,
	    c
	  );
	  exit(-3);
        }
    } // switch (c) // current character
    if (add) array_push(tokens, token);
    
  } // for (... source_code ...) // main loop

  return tokens;
}
