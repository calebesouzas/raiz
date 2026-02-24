#ifndef RAIZ_TOKENS_H
#define RAIZ_TOKENS_H

// TODO: add the remaining comments for each TOKEN_<VARIANT>
typedef enum {
  // Simple tokens:
  // exclamation mark: used to mark the end of a statement
  TOKEN_EXCLAM,     // !
  // question mark: used to trigger event 'check' at an object
  TOKEN_QUESTION,   // ?
  // at symbol: used to reference a schema (table, type, class, etc)
  TOKEN_AT,         // @
  // hashtag symbol: don't really know what i'll use it for...
  // but may be about creating a Raiz scope inside a 'math' or 'shell' block
  TOKEN_HASH,       // #
  // dollar symbol: used to create a 'shell' block in one line
  // can also be used to store stdout, stderr and stdin of a ran command
  // or just an environment variable's value as a Raiz string.
  TOKEN_DOLLAR,     // $
  // percent: probably just the normal module operator.
  // If i find another cool thing to add to this language,
  // i may change the behaviour of this symbol.
  // (then, the module operator will be a built-in function)
  TOKEN_PERCENT,    // %
  // hat: power at a 'math' block. Not sure about the 'shell' version
  TOKEN_HAT,        // ^
  // ampersand: marker for asyncronous command at 'shell' block
  TOKEN_AMPER,      // &
  // pipe: fraction separator at 'math' block or the command line '|'
  // operator at a 'shell' block
  TOKEN_PIPE,       // |
  TOKEN_TILDE,      // ~
  // asterisk: global operator at imports ('use' statements) and paths.
  TOKEN_ASTERISK,   // *
  // slash: default folder separator at paths
  // (even on Windows, when we support it)
  TOKEN_SLASH,      // /
  TOKEN_PLUS,       // +
  TOKEN_MINUS,      // -
  // collon: not sure yet, will not be used for type inference as it'll be
  // done automaticaly.
  TOKEN_COLLON,     // :
  TOKEN_COMMA,      // ,
  TOKEN_DOT,        // .
  // single equal: assign values to variables (if followed by a '?' ('=?') at
  // 'math' blocks, sets the preceeding variable to be the return value of the
  // entire 'math' expression, if there are multiple ones, a tuple with the 
  // values in the order that the variables are "checked" is returned)
  TOKEN_ASSIGN,    // =
  TOKEN_BACKSLASH, // \
  
  // Paired tokens:
  // parenthesis: 'shell' expression delimiter.
  TOKEN_LPAREN,     // (
  TOKEN_RPAREN,     // )
  // brackets: indexer operator or generic type delimiter at 'raiz' scopes
  TOKEN_LSQUARE,    // [
  TOKEN_RSQUARE,    // ]
  TOKEN_LBRACE,     // {
  TOKEN_RBRACE,     // }
  // '>' at 'shell' blocks is an operator that writes (or appends)
  // the stdout of a command into a file path specified
  TOKEN_GREATER,    // >
  TOKEN_LESSER,     // <
  
  // Composed tokens:
  TOKEN_EQUAL,      // ==
  TOKEN_NOTEQ,      // !=
  TOKEN_GT_EQ,      // >=
  TOKEN_LT_EQ,      // <=
  TOKEN_THIN_ARR,   // ->
  TOKEN_FAT_ARR,    // =>
  TOKEN_OR,         // ||
  TOKEN_AND,        // &&
  TOKEN_XOR,        // ^^ the binary XOR for boolean expressions!
  TOKEN_LSHIFT,     // <<
  TOKEN_RSHIFT,     // >>
  /* There is also something about an unsigned right shift operator with >>> */
  // TODO: search more about it (the unsigned right shift operator)

  // Literal tokens:
  TOKEN_INT,        // 123
  TOKEN_FLOAT,      // 1.75 'double' in C
  TOKEN_CHAR,       // 'A' should use 'int' to be able to use UTF-8 characters
  TOKEN_STRING,     // "Hello"

  // Keyword tokens:
  TOKEN_KEY_VAR,    // var
  TOKEN_KEY_VAL,    // val
  TOKEN_KEY_LET,    // let (used to re-assign a value to a variable)
  TOKEN_KEY_AS,     // as (for type casting)
  TOKEN_KEY_IF,     // if
  TOKEN_KEY_OR,     // or (actualy 'else if' of other languages)
  TOKEN_KEY_ELSE,   // else
  TOKEN_KEY_FOR,    // for
  TOKEN_KEY_WHILE,  // while
  TOKEN_KEY_MATCH,  // match (switch from some other languages)
  TOKEN_KEY_FUN,    // fun
  TOKEN_KEY_MET,    // met (function that uses an instance of a table object)
  TOKEN_KEY_TAB,    // tab (table)
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

#endif /* RAIZ_TOKENS_H */
