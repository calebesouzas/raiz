#ifndef RAIZ_PARSER_C
#define RAIZ_PARSER_C

int Parser_parse_nud(Expr *res, Parser *par) {
  Token tok, peeked;
  Expr *in, *value;
  int err;

  tok = Parser_cur(par);
  if (tok.kind == TOKEN_INVALID)
    return PARSER_INVALID_TOKEN;

  else if (tok.kind == TOKEN_NUMBER) {
    res->kind = EXPR_LITERAL;
    res->literal = tok.value;
  } else if (tok.kind & TOKEN_FLAG_UNARY) {
    uint8_t lbp, rbp;
    binding_power_of(&tok, &lbp, &rbp);

    Parser_advance(par);

    in = Expr_();
    err = Parser_parse_expr(in, par, rbp);
    if (err)
      return err;

    res->kind = EXPR_UNARY;
    res->unary.op = tok;
    res->unary.in = in;
  } else if (tok.kind == TOKEN_L_PAREN) {
    Parser_advance(par);

    in = Expr_();
    err = Parser_parse_expr(in, par, 0);
    if (err)
      return err;

    peeked = Parser_peek(par);
    if (peeked.kind != TOKEN_R_PAREN) {
      fprintf(stderr, "group not closed after '%s'\n", token_label(&tok));
      return PARSER_NON_CLOSED_GROUP;
    }
    Parser_advance(par); // consume ')'

    res->kind = EXPR_GROUP;
    res->group.in = in;
  } else if (tok.kind == TOKEN_IDENT) {
    res->kind = EXPR_IDENT;
    strncpy(res->ident, tok.ident, TOKEN_IDENTIFIER_SIZE);
  } else if (tok.kind == TOKEN_VAR || tok.kind == TOKEN_VAL) {
    peeked = Parser_peek(par);
    if (peeked.kind != TOKEN_IDENT) {
      fprintf(stderr, "expected identifier, found %s\n", token_label(&peeked));
      return PARSER_EXPECTED_IDENTIFIER;
    }
    char ident[TOKEN_IDENTIFIER_SIZE];
    strncpy(ident, peeked.ident, sizeof(ident));
    Parser_advance(par); // consume keyword

    peeked = Parser_peek(par);
    if (peeked.kind == TOKEN_EQUAL) {
      Parser_advance(par); // identifier
      Parser_advance(par); // '='

      value = Expr_();
      err = Parser_parse_expr(value, par, 0);
      if (err)
        return err;

      res->decl.value = value;
    } else if (tok.kind == TOKEN_VAL) {
      fprintf(stderr, "expected assignment after '%s', found %s\n",
              ident, token_label(&peeked));
      return PARSER_EXPECTED_ASSIGNMENT;
    }

    if (!((peeked = Parser_peek(par)).kind & TOKEN_FLAG_FINISHER)) {
      fprintf(stderr, "expected new line or ';', found %s\n",
              token_label(&peeked));
      return PARSER_EXPECTED_FINISH;
    }

    res->kind = EXPR_DECL;
    res->decl.kind = tok.kind;
    strncpy(res->decl.ident, ident, TOKEN_IDENTIFIER_SIZE);
  } else {
    fprintf(stderr, "unexpected token: %s\n", token_label(&tok));
    return PARSER_UNEXPECTED_TOKEN;
  }
  return 0;
}

int Parser_parse_expr(Expr *ls, Parser *par, uint8_t min_bp) {
  Expr *rs;
  Expr *res;
  Token op;
  uint8_t lbp, rbp;
  int err;

  err = Parser_parse_nud(ls, par);
  if (err)
    return err;

  while (!((op = Parser_peek(par)).kind & TOKEN_FLAG_BREAKING)) {
    if (!(op.kind & TOKEN_FLAG_OP)) {
      fprintf(stderr, "expected operator, found %s\n", token_label(&op));
      return PARSER_EXPECTED_OPERATOR;
    }

    binding_power_of(&op, &lbp, &rbp);
    if (lbp < min_bp)
      break;

    // to figure out: why do we need two advances?
    Parser_advance(par);
    Parser_advance(par);

    rs = Expr_();
    err = Parser_parse_expr(rs, par, rbp);
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

void binding_power_of(Token *op_tok, uint8_t *lbp, uint8_t *rbp) {
  const uint8_t SUM_SUB = 1;
  const uint8_t MUL_DIV = 3;
  uint32_t op = op_tok->kind;
  assert((op_tok->kind & TOKEN_FLAG_OP) && "Should provide operator token");
  switch (op) {
  case TOKEN_PLUS: case TOKEN_MINUS: *lbp = SUM_SUB; *rbp = SUM_SUB+1; break;
  case TOKEN_STAR: case TOKEN_SLASH: *lbp = MUL_DIV; *rbp = MUL_DIV+1; break;
  default: break;
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
    fprintf(stderr, "literal %d\n", root->literal);
    break;
  case EXPR_BINARY:
    fprintf(stderr, "binary, operator %s\n", token_label(&root->binary.op));

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
    fprintf(stderr, "unary, operator %s\n", token_label(&root->binary.op));

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
    fprintf(stderr, "identifier %s\n", root->ident);
    break;
  case EXPR_DECL: {
    Token tok;
    tok.kind = root->decl.kind;
    fprintf(stderr, "%s declaration\n", token_label(&tok));

    for (size_t i = 0; i < level * indent; i++) {
      fputc(' ', stderr);
    }
    fprintf(stderr, "value:\n");
    Expr_dump(root->decl.value, indent, level + 1);
  } break;
  }
}

Token Parser_cur(Parser *par) { return par->toks->dat[par->cur]; }
Token Parser_peek(Parser *par) { return par->toks->dat[par->cur+1]; }
Token Parser_next(Parser *par) { return par->toks->dat[par->cur++]; }
Token Parser_advance(Parser *par) { return par->toks->dat[++par->cur]; }

#endif // RAIZ_PARSER_C

