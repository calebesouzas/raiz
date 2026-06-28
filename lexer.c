#ifndef RAIZ_LEXER_C
#define RAIZ_LEXER_C

#define tk(k, ...)\
((Token){.kind = (k), .flags = TOKEN_FLAGS[(k)], __VA_ARGS__})

int Lexer_tokenize(Token_A *toks, char *source) {
  size_t len = strlen(source);
  size_t i;
  char c;
  for (i = 0; i < len; i++) {
    c = source[i];
    switch (c) {
    case '+': da_add(toks, tk(TOKEN_PLUS)); break;
    case '-': da_add(toks, tk(TOKEN_MINUS)); break;
    case '*': da_add(toks, tk(TOKEN_STAR)); break;
    case '/': da_add(toks, tk(TOKEN_SLASH)); break;
    case '(': da_add(toks, tk(TOKEN_L_PAREN)); break;
    case ')': da_add(toks, tk(TOKEN_R_PAREN)); break;
    case '{': da_add(toks, tk(TOKEN_L_CURLY)); break;
    case '}': da_add(toks, tk(TOKEN_R_CURLY)); break;
    case '~': da_add(toks, tk(TOKEN_TILDE)); break;
    case '=':
      if (source[i+1] == '=') {
        i++;
        da_add(toks, tk(TOKEN_EQUAL_X2));
      } else {
        da_add(toks, tk(TOKEN_EQUAL));
      }
      break;
    case '!':
      if (source[i+1] == '=') {
        i++;
        da_add(toks, tk(TOKEN_BANG_EQUAL));
      } else {
        da_add(toks, tk(TOKEN_BANG));
      }
      break;
    case '|':
      if (source[i+1] == c) {
        i++;
        da_add(toks, tk(TOKEN_PIPE_X2));
      } else {
        da_add(toks, tk(TOKEN_PIPE));
      }
      break;
    case '&':
      if (source[i+1] == c) {
        i++;
        da_add(toks, tk(TOKEN_AMPER_X2));
      } else {
        da_add(toks, tk(TOKEN_AMPER));
      }
      break;
    case '<':
      if (source[i+1] == c) {
        i++;
        da_add(toks, tk(TOKEN_LESS_X2));
      } else if (source[i+1] == '=') {
        i++;
        da_add(toks, tk(TOKEN_LESS_EQUAL));
      } else {
        da_add(toks, tk(TOKEN_LESS));
      }
      break;
    case '>':
      if (source[i+1] == c) {
        i++;
        da_add(toks, tk(TOKEN_GREAT_X2));
      } else if (source[i+1] == '=') {
        i++;
        da_add(toks, tk(TOKEN_GREAT_EQUAL));
      } else {
        da_add(toks, tk(TOKEN_GREAT));
      }
      break;
    case '\n':
      while (source[i+1] == '\n') {
        i++;
      }
      da_add(toks, tk(TOKEN_NEWLINE));
      break;
    case ' ': case '\t': case '\r': break;
    default: {
      if (isdigit(c)) {
        int num = c - '0';
        while (isdigit((c = source[i + 1])) || c == '_') {
          i++;
          if (c == '_')
            continue;
          num = (num * 10) + (c - '0');
        }
        da_add(toks, tk(TOKEN_NUMBER, .value = num));
      } else if (isalpha(c) || c == '_') {
        size_t start = i;
        Token tok;

        memset(tok.ident, 0, TOKEN_IDENTIFIER_SIZE);

        do {
          i++;
          c = source[i];
        } while (i - start < TOKEN_IDENTIFIER_SIZE && isalnum(c) || c == '_');

        if (!token_keyword(&tok, &source[start], i - start)) {
          strncpy(tok.ident, &source[start], i - start);
          tok.kind = TOKEN_IDENT;
          tok.flags = TOKEN_FLAGS[tok.kind];
        }

        if (i - start > 0)
          i--;

        da_add(toks, tok);
      } else {
        fprintf(stderr, "unhandled byte (%02x) at index [%zu]\n", c, i);
      }
    } break;
    }
  }
  da_add(toks, tk(TOKEN_EOF));
  return 0;
}

bool token_keyword(Token *tok, char *ident, size_t len) {
  size_t i, keywords_size;
  struct TokenKeywordTable key;

  keywords_size = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

  for (i = 0; i < keywords_size; i++) {
    key = KEYWORDS[i];

    if (key.len == len && strncmp(ident, key.string, key.len) == 0) {
      tok->kind = key.kind;
      tok->flags = TOKEN_FLAGS[key.kind];
      return true;
    }
  }
  return false;
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
  case TOKEN_VAR: return "var";
  case TOKEN_VAL: return "val";
  case TOKEN_IDENT: {
    size_t size = strlen("identifier ") + TOKEN_IDENTIFIER_SIZE;
    char *buf = malloc(size);
    snprintf(buf, size, "identifier %s", tok->ident);
    return buf;
  } break;
  case TOKEN_EQUAL: return "=";
  case TOKEN_EQUAL_X2: return "==";
  case TOKEN_BANG: return "!";
  case TOKEN_BANG_EQUAL: return "!=";
  case TOKEN_AMPER: return "&";
  case TOKEN_AMPER_X2: return "&&";
  case TOKEN_PIPE: return "|";
  case TOKEN_PIPE_X2: return "||";
  case TOKEN_HAT: return "^";
  case TOKEN_TILDE: return "~";
  case TOKEN_NEWLINE: return "new line";
  case TOKEN_L_CURLY: return "{";
  case TOKEN_R_CURLY: return "}";
  case TOKEN_EOF: return "EOF";
  default: fprintf(stderr, "unknown token (id %d)\n", tok->kind); return NULL;
  }
}

#endif // RAIZ_LEXER_C
