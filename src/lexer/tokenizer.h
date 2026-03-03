#ifndef RAIZ_TOKENIZER_H
#define RAIZ_TOKENIZER_H

#include "tokens.h"

Token* raiz_tokenize(char* const source_code);

#ifdef RAIZ_TOKENIZER_IMPLEMENTATION
#include "tokenizer.c"
#endif

#endif /* ifndef RAIZ_TOKENIZER */
