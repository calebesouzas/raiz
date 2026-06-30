#ifndef RAIZ_LEXER_C
#define RAIZ_LEXER_C

#define cur() Lexer_cur(lex)
#define peek() Lexer_peek(lex)
#define advance() Lexer_advance(lex)
#define active() Lexer_active(lex)
#define add(tok) Lexer_add(lex, (tok))

#define tk(k, ...)\
((Token){.kind = (k), .flags = TOKEN_FLAGS[(k)], __VA_ARGS__})

static bool skip_comments(Lexer *lex);

int Lexer_tokenize(Lexer *lex) {
  Token_A *toks = lex->toks;

  for (lex->c = lex->source[0]; Lexer_active(lex); advance()) {
    lex->start = lex->i;
    lex->columns++;
    switch (lex->c) {
    case '+': add(tk(TOKEN_PLUS)); break;
    case '-': add(tk(TOKEN_MINUS)); break;
    case '*': add(tk(TOKEN_STAR)); break;
    case '(': add(tk(TOKEN_L_PAREN)); break;
    case ')': add(tk(TOKEN_R_PAREN)); break;
    case '{': add(tk(TOKEN_L_CURLY)); break;
    case '}': add(tk(TOKEN_R_CURLY)); break;
    case '~': add(tk(TOKEN_TILDE)); break;
    case '^': add(tk(TOKEN_HAT)); break;
    case '=':
      if (peek() == '=') {
        advance();
        add(tk(TOKEN_EQUAL_X2));
      } else {
        add(tk(TOKEN_EQUAL));
      }
      break;
    case '!':
      if (peek() == '=') {
        advance();
        add(tk(TOKEN_BANG_EQUAL));
      } else {
        add(tk(TOKEN_BANG));
      }
      break;
    case '|':
      if (peek() == cur()) {
        advance();
        add(tk(TOKEN_PIPE_X2));
      } else {
        add(tk(TOKEN_PIPE));
      }
      break;
    case '&':
      if (peek() == cur()) {
        advance();
        add(tk(TOKEN_AMPER_X2));
      } else {
        add(tk(TOKEN_AMPER));
      }
      break;
    case '<':
      if (peek() == cur()) {
        advance();
        add(tk(TOKEN_LESS_X2));
      } else if (peek() == '=') {
        advance();
        add(tk(TOKEN_LESS_EQUAL));
      } else {
        add(tk(TOKEN_LESS));
      }
      break;
    case '>':
      if (peek() == cur()) {
        advance();
        add(tk(TOKEN_GREAT_X2));
      } else if (peek() == '=') {
        advance();
        add(tk(TOKEN_GREAT_EQUAL));
      } else {
        add(tk(TOKEN_GREAT));
      }
      break;
    case '\n':
      lex->columns = 1;
      while (peek() == '\n') {
        lex->lines++;
        advance();
      }
      add(tk(TOKEN_NEWLINE));
      break;
    case ' ': case '\t': case '\r': break;
    default: {
      if (isdigit(cur())) {
        int num = cur() - '0';
        while (isdigit(peek()) || peek() == '_') {
          advance();
          if (cur() == '_')
            continue;
          num = (num * 10) + (cur() - '0');
        }
        add(tk(TOKEN_NUMBER, .value = num));
      } else if (isalpha(cur()) || cur() == '_') {
        Token tok;

        memset(tok.ident, 0, TOKEN_IDENTIFIER_SIZE);

        do {
          advance();
        } while (Lexer_len(lex) < TOKEN_IDENTIFIER_SIZE
            && isalnum(cur()) || cur() == '_');

        if (!token_keyword(&tok, Lexer_point(lex), Lexer_len(lex))) {
          strncpy(tok.ident, Lexer_point(lex), Lexer_len(lex));
          tok.kind = TOKEN_IDENT;
          tok.flags = TOKEN_FLAGS[tok.kind];
        }

        if (Lexer_len(lex) > 0)
          lex->i--;

        add(tok);
      } else {
        fprintf(stderr, "unhandled byte %02x at index %zu\n", lex->c, lex->i);
        return 1;
      }
    } break;
    case '/':
      if (!skip_comments(lex)) {
        add(tk(TOKEN_SLASH));
      }
      break;
    }
  }
  add(tk(TOKEN_EOF));
  return 0;
}

bool skip_comments(Lexer *lex) {
  if (peek() == '/') {
    do advance(); while (peek() != '\n' && active());
    lex->lines++;
    lex->columns = 1;
    return true;
  } else if (peek() == '*') { //@todo suport /* */ comments
    advance();
    advance();
    for (uint32_t level = 1; level > 0 && active(); advance()) {
      if (cur() == '/' && peek() == '*')
        level++;
      else if (cur() == '*' && peek() == '/')
        level--;
    }
    return true;
  }
  return false;
}

bool token_keyword(Token *tok, char *ident, size_t len) {
  size_t i, keywords_size;
  struct TokenKeywordTable key;

  keywords_size = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

  for (i = 0; i < keywords_size; i++) {
    key = KEYWORDS[i];

    if (key.len == len && strncmp(ident, key.string, key.len) == 0) {
      tok->kind = key.kind;
      tok->value = key.value;
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

Lexer Lexer_setup(Token_A *toks, char *source) {
  Lexer lex = {0};
  lex.toks = toks;
  lex.source = source;
  lex.source_len = strlen(source);
  lex.columns = 1;
  lex.lines = 1;
  return lex;
}

char Lexer_peek(Lexer *lex) {
  return lex->source[lex->i+1];
}
char Lexer_next(Lexer *lex) {
  lex->c = lex->source[lex->i++];
  return lex->c;
}
char Lexer_advance(Lexer *lex) {
  lex->c = lex->source[++lex->i];
  return lex->c;
}
char Lexer_cur(Lexer *lex) {
  return lex->c;
}
bool Lexer_active(Lexer *lex) {
  return lex->i < lex->source_len;
}
char *Lexer_point(Lexer *lex) {
  return &lex->source[lex->start];
}
size_t Lexer_len(Lexer *lex) {
  return ((int)lex->i) - lex->start;
}

void Lexer_add(Lexer *lex, Token tok) {
  tok.lexeme = Lexer_point(lex);
  tok.len = Lexer_len(lex) + 1;
  tok.start = lex->start;
  tok.line = lex->lines;
  tok.column = lex->columns+1;
  da_add(lex->toks, tok);
}

#endif // RAIZ_LEXER_C
