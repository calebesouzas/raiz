#ifndef RAIZ_PARSER_C
#define RAIZ_PARSER_C

int Parser_parse_nud(Expr *res, Parser *par) {
  Token tok;
  Expr *in;
  int err;

  fprintf(stderr, "parse_nud()\n");
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
  } else {
    fprintf(stderr, "unexpected token: %s\n", token_label(&tok));
    return PARSER_UNEXPECTED_TOKEN;
  }
  return 0;
}

int Parser_parse_expr(Expr *res, Parser *par, uint8_t min_bp) {
  Expr *ls;
  Expr *rs;
  Token op;
  uint8_t lbp, rbp;
  int err;

  fprintf(stderr, "parse_expr()\n");
  ls = res;
  err = Parser_parse_nud(ls, par);
  if (err)
    return err;

  while (!(Parser_peek(par).kind & TOKEN_FLAG_BREAKING)) {
    op = Parser_peek(par);
    if (!(op.kind & TOKEN_FLAG_OP)) {
      fprintf(stderr, "expected operator, found %s\n", token_label(&op));
      return PARSER_EXPECTED_OPERATOR;
    }

    binding_power_of(&op, &lbp, &rbp);
    if (lbp < min_bp)
      break;

    Parser_advance(par);
    Parser_advance(par);

    rs = Expr_();
    err = Parser_parse_expr(rs, par, rbp);
    if (err)
      return err;

    res->kind = EXPR_BINARY;
    res->binary.ls = Expr_copy(ls);
    res->binary.rs = rs;
  }

  return 0;
}

int Parser_build_ast(AST *ast, Token_A *toks) {
  Parser par = {ast, toks, 0}; // `0` -> current token
  Expr *root = Expr_();
  par.ast->root = root;
  return Parser_parse_expr(root, &par, 0);
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

Token Parser_cur(Parser *par) {
  fprintf(stderr, "parser.current = %zu (%s)\n", par->cur, token_label(&par->toks->dat[par->cur]));
  return par->toks->dat[par->cur];
}
Token Parser_peek(Parser *par) {
  fprintf(stderr, "parser.peek() = %s\n", token_label(&par->toks->dat[par->cur+1]));
  return par->toks->dat[par->cur+1];
}
Token Parser_next(Parser *par) {
  fprintf(stderr, "parser.next() = %s\n", token_label(&par->toks->dat[par->cur+1]));
  return par->toks->dat[par->cur++];
}
Token Parser_advance(Parser *par) {
  Token cur, next;
  cur = par->toks->dat[par->cur];
  next = par->toks->dat[par->cur+1];
  fprintf(stderr, "parser.advance() [%zu (%s) -> %zu (%s)]\n", par->cur, token_label(&cur), par->cur+1, token_label(&next));
  return par->toks->dat[++par->cur];
}

#endif // RAIZ_PARSER_C

