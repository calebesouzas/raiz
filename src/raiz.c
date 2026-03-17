#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define RAIZ_ERROR_BUFFER_CAPACITY (1024)
// NOTE: i'm not sure if using a global buffer to store error messages is a good
// idea. But i think it is (because we don't need to pass a pointer 
// to in in every single function that can fail for some reason)
char g_raiz_error_buffer[RAIZ_ERROR_BUFFER_CAPACITY] = {0};

typedef struct {
  enum {
    // in case of unrecognized byte or invalid character in current context
    TOKEN_ERROR,
    TOKEN_PLUS,
    TOKEN_LITERAL_NUMBER,
    TOKEN_END_OF_FILE
  } kind;
  int value;
} Token;

typedef struct {
  const char* source;
  char* current;
  char* start;
} Lexer;

static inline Lexer lexer_init(char*const source) {
  return (Lexer) { .source = source, .current = source, .start = source };
}

static inline void lexer_backup(Lexer* l) {
  l->start = l->current;
}

// TODO: add checking for equality between a 'char expected' param and the
// current character, if not equal, panic on that (in debug version only)!
static inline void lexer_advance(Lexer* l) {
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
  default:
    if (g_raiz_error_buffer[0] != '\0') // means it is not empty
      memset(g_raiz_error_buffer, 0, RAIZ_ERROR_BUFFER_CAPACITY);

    // Here we are sure that the buffer will not overflow
    sprintf(g_raiz_error_buffer, "unrecognized character: %c", *l->current);
    return (Token) {.kind = TOKEN_ERROR };
  }

  return (Token) {.kind = TOKEN_END_OF_FILE};
}

int main(void) {
  // char code[] = "2 + 2";

  return 0;
}
