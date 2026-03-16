#include <ctype.h>

typedef struct {
  enum {
    TOKEN_EOF,
    TOKEN_PLUS,
    TOKEN_LITERAL_NUMBER,
  } kind;
  int value;
} Token;

typedef struct {
  const char* source;
  char* current;
  char* start;
} Lexer;

inline void lexer_backup(Lexer* l) {
  l->start = l->current;
}

// TODO: add checking for equality between a 'char expected' param and the
// current character, if not equal, panic on that (in debug version only)!
inline void lexer_advance(Lexer* l) {
  l->current++;
}

Token lexer_next_token(Lexer* l) {
  switch (*l->current) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    lexer_backup(l);
    int number = *l->current - '0';
    while (isdigit(*(++l->current))) {
      number = (number * 10) + (*l->current - '0');
      lexer_advance(l);
    }
    return (Token) {.kind = TOKEN_LITERAL_NUMBER, .value = number};
  }

  return (Token) {.kind = TOKEN_EOF};
}

int main(void) {
  // char code[] = "2 + 2";

  return 0;
}
