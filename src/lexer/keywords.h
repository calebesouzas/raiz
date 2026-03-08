#ifndef RAIZ_LEXER_KEYWORDS_H
#define RAIZ_LEXER_KEYWORDS_H
/* This header is used to declare all keyword related code at Raiz lexer */

#ifndef RAIZ_LEXER_KEYWORDS_LIST
#define RAIZ_LEXER_KEYWORDS_LIST
const char *raiz_keywords[] = { // same order as 'tokens.def' (alphabetical)
  "as", "in", "or",
  "for", "fun", "let", "met", "mut", "not", "pub", "tab", "use", "val", "var",
  "else", "math", "self",
  "event", "macro", "shell", "while",
  "onevent", "trigger"
};
#endif // ifndef RAIZ_LEXER_KEYWORDS_LIST

void raiz_set_if_is_keyword(void);

#endif // RAIZ_LEXER_KEYWORDS_H
