#ifndef RAIZ_PROGRAM_C
#define RAIZ_PROGRAM_C

int Program_build(Program *pro, Parser *par) {
  int err;
  Token tok, peeked;
  Expr *node;

  pro->toks = par->toks;

  while ((tok = Parser_next(par)).kind != TOKEN_EOF) {
    node = Expr_();

    err = Parser_parse_expr(node, par, 0);
    if (err)
      return err;

    da_add(&pro->code, node);
  }

  return 0;
}

int Program_run(Program *pro) {
  Expr **expr;
  int value;

  value = 0;
  da_for(expr, &pro->code) {
    value = eval(*expr, pro->syms);
  }

  return value;
}

void Program_debug(Program *pro, size_t indent) {
  Token *tok;
  Expr **expr;

  fprintf(stderr, "tokens = (%p) {\n", pro->toks);
  da_for(tok, pro->toks) {
    fprintf(stderr, " %s,\n", token_label(tok));
  }
  fprintf(stderr, "}\n// -------- //\n");

  fprintf(stderr, "program = (%p) {\n", &pro->code);
  da_for(expr, &pro->code) {
    Expr_dump(*expr, indent, 1);
  }
  fprintf(stderr, "}\n// -------- //\n");
}

Program Program_setup(Symbol_A *syms, Parser *par) {
  Program pro = {0};
  pro.toks = par->toks;
  pro.syms = syms;
  return pro;
}

#endif // RAIZ_PROGRAM_C
