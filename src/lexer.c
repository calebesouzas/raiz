#include "lexer.h"

#include "raiz_debug/logs.h"
#include "raiz_helpers/switch.h" // CASE (auto break) macro

#include "raiz_arrays.h"
#include "raiz_strings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint; // alias because it's too long

#define is_next(character) (source_code[index + 1] == character)

#define is_number(c) (c >= '0' && c <= '9')
#define is_alpha(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

#define is_identstart(c) (is_alpha(c) || c == '_' )
#define is_identchar(c) (is_identstart(c) || is_number(c))

#define set(kind) (*token) = set_simple_token(kind, 1, index);
#define setn(kind, len) (*token) = set_simple_token(kind, len, index);

static inline Token set_simple_token(TokenKind kind, uint len, uint start) {
  return (Token) {
    .kind = kind,
    .len = len,
    .start = start,
  };
}

// NOTE: could implement a hash table instead of iterating through
// each keyword and comparing it with 'word'
void set_if_is_keyword(char*const word, uint len, Token* token) {
  const char *keywords[] = { // same order as in 'tokens.def'
    "as", "in", "or",
    "for", "fun", "let", "met", "mut",
    "not", "pub", "tab", "use", "val", "var",
    "else", "math", "self",
    "event", "macro", "shell", "while",
    "onevent", "trigger"
  };

  // i know there is a better way, but i don't know this way (not yet)
  uint keywords_start = (uint) RAIZ_TOKEN_LITERAL_STRING + 1;

  for (uint i = 0; i < (sizeof(keywords) / sizeof(keywords[0])); ++i) {
    if (strncmp(word, keywords[i], len) == 0) {
      token->kind = (TokenKind)(i + keywords_start); // hope it does work...
      return;
    }
  }

  token->len = len;
  token->kind = RAIZ_TOKEN_IDENTIFIER;
  string_push_slice(token->data.s_val, word, len);
}

int handle_simple_token(char*const source_code, uint index, Token* token) {
  int result = 1;
  switch (source_code[index]) {
  // TODO: handle one, two or three symbol sequence tokens
  CASE('!',
    if is_next('=') {
      setn(RAIZ_TOKEN_NOT_EQUAL, 2)
    } else {
      set(RAIZ_TOKEN_EXCLAMATION);
    })
  CASE('@', set(RAIZ_TOKEN_AT))
  CASE('#', set(RAIZ_TOKEN_HASH))
  CASE('$', set(RAIZ_TOKEN_DOLLAR))
  CASE('%', set(RAIZ_TOKEN_PERCENT))
  CASE('^',
    if is_next('^') {
      setn(RAIZ_TOKEN_DOUBLE_HAT, 2)
    } else {
      set(RAIZ_TOKEN_HAT)
    })
  CASE('&',
    if is_next('&') {
      setn(RAIZ_TOKEN_DOUBLE_AMPERSAND, 2)
    } else {
      set(RAIZ_TOKEN_AMPERSAND)
    })
  CASE('*', set(RAIZ_TOKEN_STAR))
  CASE('(', set(RAIZ_TOKEN_OPEN_PARENTHESES))
  CASE(')', set(RAIZ_TOKEN_CLOSE_PARENTHESES))
  CASE('[', set(RAIZ_TOKEN_OPEN_BRACKETS))
  CASE(']', set(RAIZ_TOKEN_CLOSE_BRACKETS))
  CASE('{', set(RAIZ_TOKEN_OPEN_CURLY_BRACES))
  CASE('}', set(RAIZ_TOKEN_CLOSE_CURLY_BRACES))
  CASE('-',
    if is_next('>') {
      setn(RAIZ_TOKEN_THIN_ARROW, 2)
    } else if is_next('`') {
      setn(RAIZ_TOKEN_HALF_THIN_ARROW, 2)
    } else {
      set(RAIZ_TOKEN_MINUS)
    })
  CASE('+', set(RAIZ_TOKEN_PLUS))
  CASE('=',
    if is_next('=') {
      setn(RAIZ_TOKEN_EQUAL, 2)
    } else if is_next('>') {
      setn(RAIZ_TOKEN_FAT_ARROW, 2)
    } else {
      set(RAIZ_TOKEN_ASSIGN)
    })
  CASE(',', set(RAIZ_TOKEN_COMMA))
  CASE('.', set(RAIZ_TOKEN_DOT))
  CASE(':', set(RAIZ_TOKEN_COLLON))
  CASE('/', set(RAIZ_TOKEN_SLASH))
  CASE('?', set(RAIZ_TOKEN_QUESTION))
  CASE('~',
    if is_next('>') {
      setn(RAIZ_TOKEN_WAVE_ARROW, 2)
    } else {
      set(RAIZ_TOKEN_TILDE)
    })
  CASE('>',
    if is_next('=') {
      setn(RAIZ_TOKEN_GREATER_OR_EQUAL, 2)
    } else if is_next('>') {
      setn(RAIZ_TOKEN_DOUBLE_GREATER, 2)
    } else {
      set(RAIZ_TOKEN_GREATER_THAN)
    })
  CASE('<',
    if is_next('<') {
      setn(RAIZ_TOKEN_DOUBLE_LESS, 2)
    } else if is_next('=') {
      setn(RAIZ_TOKEN_LESS_OR_EQUAL, 2)
    } else {
      set(RAIZ_TOKEN_LESS_THAN)
    })
  CASE('|',
    if is_next('|') {
      setn(RAIZ_TOKEN_DOUBLE_PIPE, 2)
    } else {
      set(RAIZ_TOKEN_PIPE)
    })
  default:
    result = 0;
    break;
  }

  return result;
}

Token* raiz_tokenize(char* const source_code) {
  Token* tokens = NULL;
  Token token;

  uint line = 1;
  uint column = 0;

  for (uint i = 0; source_code[i]; ++i) {
    short add = 1;
    column++;
    token.start = i;
    token.column = column;
    token.line = line;

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
      if (handle_simple_token(source_code, i, &token)) {}
      // NOTE: only 32 bit integer numbers are handled
      // and there are no prefixes or suffixes
      // (such as '0x', '0b' prefixes or 'u' and 'f' suffixes)
      else if (is_number(source_code[i])) {
        // TODO: handle floating numbers
        int number = 0;
        uint start = i;

        // if entered this 'if' block, will run the loop below at least once
        // Then, the number and it's length are calculated correctly
        for (; is_number(source_code[i]); ++i) {
          // TODO: check for and skip '_'s for better reading of the number
          number = (number * 10) + (source_code[i] - '0');
        }

        token.kind = RAIZ_TOKEN_LITERAL_INT;
        token.data.i_val = number;
        token.len = i - start;

        i--; // don't skip chars after numbers!
      } else if (is_identstart(source_code[i])) {
        uint start = i;

        // get identifier length
        for (; is_identchar(source_code[i]); ++i);

        // NOTE: i could implement a hash map for performance and DX reasons
        set_if_is_keyword(source_code + start, i, &token);
      } else {
        fprintf(
          stderr,
          "raiz: invalid character at %u:%u (%s): %c\n",
          line,
          column + 1,
          "only ASCII characters are accepted outside strings",
          source_code[i]
        );
        array_free(tokens);
        return NULL;
      }
      break; // default case
    } // switch (source_code[i]) // current character

    const char* token_variant_names[] = {
      #define RAIZ_DEFINE_TOKEN(variant, string) #variant,
      #include "tokens.def"
      #undef RAIZ_DEFINE_TOKEN
      NULL
    };
    printf("[RAIZ]: add token (%s)", token_variant_names[token.kind]);

    switch (token.kind) {
    case RAIZ_TOKEN_LITERAL_CHAR:
      printf(" < '%s' >\n", token.data.c_val);
      break;
    case RAIZ_TOKEN_LITERAL_INT:
      printf(" < %d >\n", token.data.i_val);
      break;
    case RAIZ_TOKEN_LITERAL_STRING:
      printf(" < \"%s\" >\n", token.data.s_val);
      break;
    default:
      printf("\n");
      break;
    }

    if (add) array_push(tokens, token);

  } // for (... source_code ...) // main loop

  return tokens;
}
