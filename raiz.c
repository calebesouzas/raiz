#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
/*
 Warning: this codebase is probably terrible...
But maybe you can find something interesting in here, I really don't know
*/

#define da_make(TypeName, TypeItems)\
  typedef struct {\
    size_t len, cap;\
    TypeItems dat;\
  } TypeName
#define DEFAULT_DA_CAP 256
#define da_size(da) (sizeof((da)->dat[0])*(da)->cap)
// you know what? Yep, I just stole it from an "AI" or someone in a forum
#define da_for(item, da)\
  for (size_t i_##item = 0;\
       (i_##item < (da)->len) && ((item) = &(da)->dat[i_##item]);\
       i_##item++)
#define da_add(da, val)\
  do{\
    if ((da)->cap == 0) (da)->cap = DEFAULT_DA_CAP;\
    if ((da)->dat == NULL) (da)->dat = malloc(da_size(da));\
    else if ((da)->len >= (da)->cap) {\
      (da)->cap *= 2;\
      (da)->dat = realloc((da)->dat, da_size(da));\
    }\
    if ((da)->dat != NULL) (da)->dat[(da)->len++] = (val);\
  } while (0)

typedef struct {
  enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_EOF,
  } kind;
  int value; // if it's `TOKEN_NUMBER`
} Token;
da_make(Token_A, Token*);

int Lexer_tokenize(Token_A *toks, char *source);

int main(int argc, char **argv) {
#if 0
  if (argc <= 1) {
    printf("usage: %s <file>\n", argv[0]);
    printf("<file>: text file containing Raiz code\n");
    return 1;
  }
#endif
  Token_A toks = {0};
  if (Lexer_tokenize(&toks, "69 + 1337\n") == 0) {
    Token *tok = NULL;
    da_for(tok, &toks) {
      printf("Token [%d].kind = %d", i_tok, tok->kind);
      if (tok->kind == TOKEN_NUMBER)
        printf(" (%d)\n", tok->value);
      else
        printf("\n");
    }
  }
  return 0;
}

int Lexer_tokenize(Token_A *toks, char *source) {
  size_t len = strlen(source);
  size_t i;
  char c;
  for (i = 0; i < len; i++) {
    c = source[i];
    switch (c) {
    case '+': da_add(toks, (Token){.kind = TOKEN_PLUS}); break;
    case '-': da_add(toks, (Token){.kind = TOKEN_MINUS}); break;
    case ' ': case '\n': case '\t': case '\r': break;
    default: {
      if (isdigit(c)) {
        int num = c - '0';
        while (isdigit((c = source[i + 1])) || c == '_') {
	  i++;
          if (c == '_')
            continue;
          num = (num * 10) + (c - '0');
	}
        da_add(toks, ((Token){.kind = TOKEN_NUMBER, .value = num}));
      } else {
        fprintf(stderr, "unhandled byte (%02x) at index [%d]\n", c, i);
      }
    } break;
    }
  }
  da_add(toks, (Token){.kind=TOKEN_EOF});
  return 0;
}
