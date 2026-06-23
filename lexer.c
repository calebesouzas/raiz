#ifndef RAIZ_LEXER_C
#define RAIZ_LEXER_C

int Lexer_tokenize(Token_A *toks, char *source) {
  size_t len = strlen(source);
  size_t i;
  char c;
  for (i = 0; i < len; i++) {
    c = source[i];
    switch (c) {
    case '+': da_add(toks, (Token){.kind = TOKEN_PLUS}); break;
    case '-': da_add(toks, (Token){.kind = TOKEN_MINUS}); break;
    case '*': da_add(toks, (Token){.kind = TOKEN_STAR}); break;
    case '/': da_add(toks, (Token){.kind = TOKEN_SLASH}); break;
    case '(': da_add(toks, (Token){.kind = TOKEN_L_PAREN}); break;
    case ')': da_add(toks, (Token){.kind = TOKEN_R_PAREN}); break;
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
        fprintf(stderr, "unhandled byte (%02x) at index [%zu]\n", c, i);
      }
    } break;
    }
  }
  da_add(toks, (Token){.kind = TOKEN_EOF});
  return 0;
}

char *token_label(Token *tok) {
  switch (tok->kind) {
  case TOKEN_INVALID: return "invalid";
  case TOKEN_NUMBER: {
    char *buf = malloc(32);
    snprintf(buf, 32, "number %d", tok->value);
    return buf;
  } break;
  case TOKEN_PLUS: return "+";
  case TOKEN_MINUS: return "-";
  case TOKEN_STAR: return "*";
  case TOKEN_SLASH: return "/";
  case TOKEN_L_PAREN: return "(";
  case TOKEN_R_PAREN: return ")";
  case TOKEN_EOF: return "EOF";
  default: fprintf(stderr, "unknown token (id %d)\n", tok->kind); return NULL;
  }
}

#endif // RAIZ_LEXER_C
