#ifndef RAIZ_TOKENS_H
#define RAIZ_TOKENS_H

#include "../core/positions.h"

// TODO: add the remaining comments for each TOKEN_<VARIANT>
typedef enum {
  // Simple tokens:
  // exclamation mark: used to mark the end of a statement
  TOKEN_EXCLAM,     // ! ok
  // question mark: used to trigger event 'check' at an object
  TOKEN_QUESTION,   // ? ok
  // at symbol: used to reference a schema (table, type, class, etc)
  TOKEN_AT,         // @ ok
  // hashtag symbol: don't really know what i'll use it for...
  // but may be about creating a Raiz scope inside a 'math' or 'shell' block
  TOKEN_HASH,       // # ok
  // dollar symbol: used to create a 'shell' block in one line
  // can also be used to store stdout, stderr and stdin of a ran command
  // or just an environment variable's value as a Raiz string.
  TOKEN_DOLLAR,     // $ ok
  // percent: probably just the normal module operator.
  // If i find another cool thing to add to this language,
  // i may change the behaviour of this symbol.
  // (then, the module operator will be a built-in function)
  TOKEN_PERCENT,    // % ok
  // grave accent: not.sure about what i'll do. May be.something related to
  // regex and patterns at strings...
  TOKEN_GRAVE,      // ` ok
  // hat: power at a 'math' block. Not sure about the 'shell' version
  TOKEN_HAT,        // ^ ok
  // ampersand: marker for asyncronous command at 'shell' block
  TOKEN_AMPER,      // & ok
  // pipe: fraction separator at 'math' block or the command line '|'
  // operator at a 'shell' block
  TOKEN_PIPE,       // | ok
  TOKEN_TILDE,      // ~ ok
  // asterisk: global operator at imports ('use' statements) and paths.
  TOKEN_ASTERISK,   // * ok
  // slash: default folder separator at paths
  // (even on Windows, when we support it)
  TOKEN_SLASH,      // / ok
  TOKEN_PLUS,       // + ok
  TOKEN_MINUS,      // - ok
  // collon: not sure yet, will not be used for type inference as it'll be
  // done automaticaly.
  TOKEN_COLLON,     // : ok
  TOKEN_COMMA,      // , ok
  TOKEN_DOT,        // . ok
  // single equal: assign values to variables (if followed by a '?' ('=?') at
  // 'math' blocks, sets the preceeding variable to be the return value of the
  // entire 'math' expression, if there are multiple ones, a tuple with the 
  // values in the order that the variables are "checked" is returned)
  TOKEN_ASSIGN,    // = ok
  TOKEN_BACKSLASH, // \ ok
  
  // Paired tokens:
  // parenthesis: 'shell' expression delimiter.
  TOKEN_LPAREN,     // ( ok
  TOKEN_RPAREN,     // ) ok
  // brackets: indexer operator or generic type delimiter at 'raiz' scopes
  TOKEN_LSQUARE,    // [ ok
  TOKEN_RSQUARE,    // ] ok
  TOKEN_LBRACE,     // { ok
  TOKEN_RBRACE,     // } ok
  // '>' at 'shell' blocks is an operator that writes (or appends)
  // the stdout of a command into a file path specified
  TOKEN_GREATER,    // > ok
  TOKEN_LESSER,     // < ok
  
  // Composed tokens:
  TOKEN_EQUAL,      // == ok
  TOKEN_NOTEQ,      // != ok
  TOKEN_GT_EQ,      // >= ok
  TOKEN_LT_EQ,      // <= ok
  TOKEN_THIN_ARR,   // -> ok
  TOKEN_FAT_ARR,    // => ok
  TOKEN_WAVE_ARR,   // ~> ok
  TOKEN_OR,         // || ok
  TOKEN_AND,        // && ok
  TOKEN_XOR,        // ^^ ok - the binary XOR for boolean expressions!
  TOKEN_LSHIFT,     // << ok
  TOKEN_RSHIFT,     // >> ok
  /* There is also something about an unsigned right shift operator with >>> */
  // TODO: search more about it (the unsigned right shift operator)

  // Literal tokens:
  TOKEN_INT,        // 123
  TOKEN_FLOAT,      // 1.75 'double' in C
  TOKEN_CHAR,       // 'A' should use 'int' to be able to use UTF-8 characters
  TOKEN_STRING,     // "Hello"

  // Keyword tokens:
  TOKEN_KEY_MATH,   // math
  TOKEN_KEY_SHELL,  // shell
  TOKEN_KEY_VAR,    // var
  TOKEN_KEY_VAL,    // val
  TOKEN_KEY_LET,    // let (used to re-assign a value to a variable)
  TOKEN_KEY_AS,     // as (for type casting)
  TOKEN_KEY_IF,     // if
  TOKEN_KEY_OR,     // or (actualy 'else if' of other languages)
  TOKEN_KEY_ELSE,   // else
  TOKEN_KEY_FOR,    // for
  TOKEN_KEY_WHILE,  // while
  TOKEN_KEY_IN,     // in
  TOKEN_KEY_MATCH,  // match (switch from some other languages)
  TOKEN_KEY_FUN,    // fun
  TOKEN_KEY_MET,    // met (function that uses an instance of a table object)
  TOKEN_KEY_TAB,    // tab (table)
  TOKEN_KEY_SELF,   // self (some languages use 'this')
  TOKEN_KEY_ONEV,   // onevent
                    // (special function that handles a triggered event)
  TOKEN_KEY_EVENT,  // event (for event declaration)
  TOKEN_KEY_TRIG,   // trigger (to trigger an event at an object or globaly)
  TOKEN_KEY_PUB,    // pub (abreviation for public)
  TOKEN_KEY_USE,    // use

  // Identifier token:
  TOKEN_IDENTIFIER,
  // An identifier is any alphanumeric or '_' character sequence, not starting
  // with a numeric character and containing only ASCII characters. If using
  // a keyword as an identifier is needed, prefix it with a '\' 
  // (the character for such task may be changed)
} TokenKind;

typedef struct {
  TokenKind kind;
  unsigned int len;
  void *value;
  Position pos;
} Token;

#endif /* RAIZ_TOKENS_H */
