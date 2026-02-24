#ifndef RAIZ_TOKENS_H
#define RAIZ_TOKENS_H

// TODO: add the remaining comments for each TOKEN_<VARIANT>
typedef enum {
  // Exclamation mark: used to mark the end of a statement
  TOKEN_EXCLAM,   // !
  // Question mark: used to trigger event 'check' at an object
  TOKEN_QUESTION, // ?
  // At symbol: used to reference a schema (table, type, class, etc)
  TOKEN_AT,       // @
  // Hashtag symbol: don't really know what i'll use it for...
  // but may be about creating a Raiz scope inside a 'math' or 'shell' block
  TOKEN_HASH,     // #
  // Dollar symbol: used to create a 'shell' block in one line
  // can also be used to store stdout, stderr and stdin of a ran command
  // or just an environment variable's value as a Raiz string.
  TOKEN_DOLLAR,   // $
  // Percent: probably just the normal module operator.
  // If i find another cool thing to add to this language,
  // i may change the behaviour of this symbol.
  // (then, the module operator will be a built-in function)
  TOKEN_PERCENT,  // %
  // Hat: used for bitwise XOR operation at 'raiz' block
  // or power at a 'math' block. Not sure about the 'shell' version
  TOKEN_HAT,      // ^
  // Ampersand: bitwise AND operation at 'raiz' block or
  // a marker for asyncronous command at 'shell' block
  TOKEN_AMPER,    // &
  // Pipe: bitwise OR operation at 'raiz' block,
  // fraction separator at 'math' block or the command line '|'
  // operator at a 'shell' block
  TOKEN_PIPE,     // |
  TOKEN_TILDE,    // ~
  TOKEN_ASTERISK, // *
  TOKEN_SLASH,    // /
  TOKEN_PLUS,     // +
  TOKEN_MINUS,    // -
  TOKEN_COLLON,   // :
  TOKEN_COMMA,    // ,
  TOKEN_DOT,      // .
  TOKEN_LPAREN,   // (
  TOKEN_RPAREN,   // )
  TOKEN_LSQUARE,  // [
  TOKEN_RSQUARE,  // ]
  TOKEN_LBRACE,   // {
  TOKEN_RBRACE,   // }
  TOKEN_GREATER,  // >
  TOKEN_LESSER,   // <
  TOKEN_ASSIGN,   // =
  TOKEN_EQUAL,    // ==
  TOKEN_NOTEQ,    // !=
  TOKEN_GT_EQ,    // >=
  TOKEN_LT_EQ,    // <=
  TOKEN_THIN_ARR, // ->
  TOKEN_FAT_ARR,  // =>
  TOKEN_OR,       // ||
  TOKEN_AND,      // &&
  TOKEN_XOR,      // ^^
  TOKEN_LSHIFT,   // <<
  TOKEN_RSHIFT,   // >>
  /* There is also something about an unsigned right shift operator with >>> */
  // TODO: search more about it (the unsigned right shift operator)
} TokenKind;

#endif /* RAIZ_TOKENS_H */
