#ifndef RAIZ_PARSER_C
#define RAIZ_PARSER_C

int Parser_parse_nud(Expr *res, Parser *par) {
  Token *tok, *peeked;
  Expr *in, *value, *line;
  int err;

  tok = Parser_cur(par);
  if (tok->kind == TOKEN_INVALID)
    return PARSER_INVALID_TOKEN;

  else if (tok->flags & TOKEN_FLAG_CONST) {
    res->kind = EXPR_LITERAL;
    res->literal = tok;
  } else if (tok->flags & TOKEN_FLAG_UNARY) {
    uint8_t bp = get_binding_power(tok->kind);
    if (tok->flags & TOKEN_FLAG_RIGHT)
      bp -= 1;

    Parser_advance(par);

    in = Expr_();
    err = Parser_parse_expr(in, par, bp);
    if (err)
      return err;

    res->kind = EXPR_UNARY;
    res->unary.op = tok;
    res->unary.in = in;
  } else if (tok->kind == TOKEN_L_PAREN) {
    Parser_advance(par);

    in = Expr_();
    err = Parser_parse_expr(in, par, 0);
    if (err)
      return err;

    peeked = Parser_peek(par);
    if (peeked->kind != TOKEN_R_PAREN) {
      fprintf(stderr, "group not closed after '%s'\n", token_label(tok));
      return PARSER_NOT_CLOSED_GROUP;
    }
    Parser_advance(par); // consume ')'

    res->kind = EXPR_GROUP;
    res->group.in = in;
  } else if (tok->kind == TOKEN_IDENT) {
    res->kind = EXPR_IDENT;
    res->ident = tok;
  } else if (tok->kind == TOKEN_L_CURLY) {
    Parser_advance(par); // `{`
    do {
      line = Expr_();

      err = Parser_parse_line(line, par);
      if (err)
        return err;

      peeked = Parser_peek(par);

      da_add(&res->block, line);
    } while (par->cur < par->toks->len && peeked->kind != TOKEN_R_CURLY);

    if (peeked->kind != TOKEN_R_CURLY) {
      fprintf(stderr, "not closed block\n");
      //@todo print start line when we track line numbers
      return PARSER_NOT_CLOSED_BLOCK;
    }
    Parser_advance(par); // before '}'

    res->kind = EXPR_BLOCK;
  } else if (tok->kind == TOKEN_HAT) {
    uint32_t level = 0;
    do {
      level++;
      tok = Parser_advance(par);
    } while (Parser_cur(par)->kind == TOKEN_HAT);
    if (tok->kind != TOKEN_IDENT) {
      fprintf(stderr, "expected identifier, found %s\n", token_label(tok));
      return PARSER_EXPECTED_IDENTIFIER;
    }

    res->kind = EXPR_PARENT;
    res->parent.level = level;
    res->parent.ident = tok;
  } else {
    fprintf(stderr, "unexpected token: %s\n", token_label(tok));
    return PARSER_UNEXPECTED_TOKEN;
  }
  return 0;
}

int Parser_parse_expr(Expr *ls, Parser *par, uint8_t min_bp) {
  Expr *rs;
  Expr *res;
  Token *op;
  uint8_t bp;
  int err;

  err = Parser_parse_nud(ls, par);
  if (err)
    return err;

  while (!((op = Parser_peek(par))->flags & TOKEN_FLAG_BREAKING)) {
    if (!(op->flags & TOKEN_FLAG_OP)) {
      fprintf(stderr, "expected operator, found %s\n", token_label(op));
      return PARSER_EXPECTED_OPERATOR;
    }

    bp = get_binding_power(op->kind);
    if (op->flags & TOKEN_FLAG_RIGHT)
      bp -= 1;

    if (bp < min_bp)
      break;

    // to figure out: why do we need two advances?
    Parser_advance(par);
    Parser_advance(par);

    rs = Expr_();
    err = Parser_parse_expr(rs, par, bp);
    if (err)
      return err;

    res = Expr_();
    res->kind = EXPR_BINARY;
    res->binary.ls = Expr_copy(ls);
    res->binary.op = op;
    res->binary.rs = rs;
    memcpy(ls, res, sizeof(*ls));
  }

  return 0;
}

int Parser_parse_line(Expr *res, Parser *par) {
  int err;
  Token *tok, *peeked;
  char ident[TOKEN_IDENTIFIER_SIZE];
  Expr *value;

  tok = Parser_cur(par);
  while (tok->flags & TOKEN_FLAG_FINISHER)
    tok = Parser_advance(par);

  if (!(tok->flags & TOKEN_FLAG_STARTER)) {
    err = Parser_parse_expr(res, par, 0);
    if (err)
      return err;

    goto finish_line;
  }
  switch (tok->kind) {
  case TOKEN_VAR:
  case TOKEN_VAL:
    peeked = Parser_peek(par);
    if (peeked->kind != TOKEN_IDENT) {
      fprintf(stderr, "expected identifier, found %s\n", token_label(peeked));
      return PARSER_EXPECTED_IDENTIFIER;
    }
    strncpy(ident, peeked->ident, sizeof(ident));
    Parser_advance(par); // consume keyword

    peeked = Parser_peek(par);
    if (peeked->kind == TOKEN_EQUAL) {
      Parser_advance(par); // identifier
      Parser_advance(par); // '='

      value = Expr_();
      err = Parser_parse_expr(value, par, 0);
      if (err)
        return err;

      res->decl.value = value;
    } else if (tok->kind == TOKEN_VAL) {
      fprintf(stderr, "expected assignment after '%s', found %s\n",
              ident, token_label(peeked));
      return PARSER_EXPECTED_ASSIGNMENT;
    }

    res->kind = EXPR_DECL;
    res->decl.tok = tok;
    res->decl.ident = tok + 1;
    break; // case VAR or VAL
  default: UNREACHABLE("token %s\n", token_label(tok));
  }

finish_line:
  if (!((peeked = Parser_peek(par))->flags & TOKEN_FLAG_FINISHER)) {
    fprintf(stderr, "expected new line or ';', found %s\n",
            token_label(peeked));
    return PARSER_EXPECTED_FINISH;
  }

  // Parser_advance(par);
  Parser_advance(par);
  return 0;
}

Parser Parser_setup(Token_A *toks) {
  return (Parser) {.toks = toks, .cur = 0};
}

void Parser_debug(Parser *par) {
  fprintf(stderr, "parser->toks = (%p) {\n", par->ast);
  Token *tok;
  da_for(tok, par->toks) {
    fprintf(stderr, "  %s,", token_label(tok));
    if (i_tok == par->cur)
      fprintf(stderr, " // current\n");
    else
      fprintf(stderr, "\n");
  }
  fprintf(stderr, "}\n// ------ //\n");

  fprintf(stderr, "parser->ast = (%p):\n", par->ast);
  Expr_dump(par->ast, 2, 0);
  fprintf(stderr, "// ------ //\n");
}

uint8_t get_binding_power(enum TokenKind kind) {
  switch(kind) {
  case TOKEN_EQUAL:       return 10;  // =
  case TOKEN_BANG:        return 20;  // !
  case TOKEN_PIPE_X2:     return 30;  // ||
  case TOKEN_AMPER_X2:    return 40;  // &&
  case TOKEN_TILDE:       return 50;  // ~
  case TOKEN_PIPE:        return 60;  // |
  case TOKEN_HAT:         return 70;  // ^
  case TOKEN_AMPER:       return 80;  // &
  case TOKEN_EQUAL_X2:                // ==
  case TOKEN_BANG_EQUAL:  return 90;  // !=
  case TOKEN_LESS:                    // <
  case TOKEN_LESS_EQUAL:              // <=
  case TOKEN_GREAT:                   // >
  case TOKEN_GREAT_EQUAL: return 100; // >=
  case TOKEN_LESS_X2:                 // <<
  case TOKEN_GREAT_X2:    return 110; // >>
  case TOKEN_PLUS:                    // +
  case TOKEN_MINUS:       return 120; // -
  case TOKEN_STAR:                    // *
  case TOKEN_SLASH:       return 130; // /
  default: PANIC("token is not an operator or is unhandled (id %d)\n", kind);
  }
}

Expr *Expr_(void) {
  Expr *e = malloc(sizeof(*e));
  if (e)
    memset(e, 0, sizeof(*e));
  return e;
}

Expr *Expr_copy(Expr *src) {
  Expr *e = malloc(sizeof(*e));
  if (e)
    memcpy(e, src, sizeof(*e));
  return e;
}

void Expr_free(Expr *node) {
  Expr **expr;

  if (node == NULL)
    return;

  switch (node->kind) {
  case EXPR_LITERAL:
  case EXPR_IDENT:
  case EXPR_PARENT:
    break;
  case EXPR_BINARY:
    Expr_free(node->binary.ls);
    Expr_free(node->binary.rs);
    break;
  case EXPR_UNARY:
    Expr_free(node->unary.in);
    break;
  case EXPR_GROUP:
    Expr_free(node->group.in);
    break;
  case EXPR_DECL:
    if (node->decl.value != NULL)
      Expr_free(node->decl.value);
    break;
  case EXPR_BLOCK:
    da_for(expr, &node->block) {
      Expr_free(*expr);
    }
    break;
  }
  free(node);
}

void Expr_dump(Expr *root, size_t indent, size_t level) {
  for (size_t i = 0; i < level * indent; i++) {
    fputc(' ', stderr);
  }
  if (!root) {
    fprintf(stderr, "null expression\n");
    return;
  }
  switch (root->kind) {
  case EXPR_LITERAL:
    fprintf(stderr, "literal %d\n", root->literal->value);
    break;
  case EXPR_BINARY:
    fprintf(stderr, "binary, operator %s\n", token_label(root->binary.op));

    for (size_t i = 0; i < level * indent; i++) {
      fputc(' ', stderr);
    }
    fprintf(stderr, "left side:\n");
    Expr_dump(root->binary.ls, indent, level + 1);

    for (size_t i = 0; i < level * indent; i++) {
      fputc(' ', stderr);
    }
    fprintf(stderr, "right side:\n");
    Expr_dump(root->binary.rs, indent, level + 1);
    break;
  case EXPR_UNARY:
    fprintf(stderr, "unary, operator %s\n", token_label(root->binary.op));

    for (size_t i = 0; i < level * indent; i++) {
      fputc(' ', stderr);
    }
    fprintf(stderr, "inner side:\n");
    Expr_dump(root->unary.in, indent, level + 1);
    break;
  case EXPR_GROUP:
    fprintf(stderr, "group\n");

    for (size_t i = 0; i < level * indent; i++) {
      fputc(' ', stderr);
    }
    fprintf(stderr, "inner side:\n");
    Expr_dump(root->group.in, indent, level + 1);
    break;
  case EXPR_IDENT:
    fprintf(stderr, "identifier %.*s\n",
            size_t_int(root->ident->len, INT_MAX), root->ident->lexeme);
    break;
  case EXPR_DECL: {
    fprintf(stderr, "%s declaration\n", token_label(root->decl.tok));

    for (size_t i = 0; i < level * indent; i++) {
      fputc(' ', stderr);
    }
    fprintf(stderr, "value:\n");
    Expr_dump(root->decl.value, indent, level + 1);
  } break;
  default: UNREACHABLE("expression kind (%d)\n", root->kind);
  }
}

Token *Parser_cur(Parser *par) { return &par->toks->dat[par->cur]; }
Token *Parser_peek(Parser *par) { return &par->toks->dat[par->cur+1]; }
Token *Parser_next(Parser *par) { return &par->toks->dat[par->cur++]; }
Token *Parser_advance(Parser *par) { return &par->toks->dat[++par->cur]; }

#endif // RAIZ_PARSER_C

