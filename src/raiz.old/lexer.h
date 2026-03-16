#ifndef RAIZ_LEXER_H
#define RAIZ_LEXER_H

// TODO: add the remaining comments for each TOKEN_<VARIANT>
typedef enum {
  #define RAIZ_DEFINE_TOKEN(variant, string) variant,
  #include "tokens.def"
  #undef RAIZ_DEFINE_TOKEN
  RAIZ_TOKEN_COUNT
} Raiz_TokenKind;

typedef struct {
  Raiz_TokenKind kind;
  size_t len, start, line, column;
  union {
    // NOTE: use the offsets (start, end/len) for string literals
    // TODO: implement some kind of global strings hash map
    // where every string slice you find, instead of malloc(), copying it and
    // free() everywhere, you add the string as a value in the hash map,
    // and store the hash number on the thing which is referencing that string
    char c_val; // we'll use only ASCII for now
    int i_val;
    // TODO: add support for unsigned integer literals
    double d_val; // TODO: add support for floating number literals
    bool b_val;
  } data;
} Raiz_Token;

typedef struct {
  size_t count;
  size_t capacity;
  Raiz_Token* items;
} Raiz_Tokens;

// TODO: add support for literals:
// (string, char, hexadecimal, octodecimal, unsigned int, bool, float, etc)
#define RAIZ_LEXER_STATE_X_MACRO_TABLE \
  X(RAIZ_LEXER_STATE_NORMAL, "LexerState.Normal"), \
  X(RAIZ_LEXER_STATE_INTEGER, "LexerState.Integer"), \
  X(RAIZ_LEXER_STATE_IDENTIFIER, "LexerState.Identifier"), \
  X(RAIZ_LEXER_STATE_TYPE_IDENTIFIER, "LexerState.TypeIdentifier"),

typedef enum {
  #define X(variant, stringver) variant,
  RAIZ_LEXER_STATE_X_MACRO_TABLE
  #undef X
} Raiz_LexerState;

typedef struct {
  Raiz_LexerState state;
  size_t current, start, start_line, start_column, line, column;
  Raiz_Tokens tokens;
} Raiz_Lexer;

Raiz_Token* raiz_tokenize(char* const source_code);

#endif // RAIZ_LEXER_TOKENS_H
