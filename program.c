#ifndef RAIZ_PROGRAM_C
#define RAIZ_PROGRAM_C

void breakpoint(void) {
  for (int i = 0; i < 3; i++);
}

int Program_build(Program *pro) {
  int err;
  Token tok, peeked;
  Expr *node;
  Parser *par;

  par = pro->par;

  while ((tok = Parser_cur(par)).kind != TOKEN_EOF) {
    node = Expr_();

    err = Parser_parse_line(node, par);
    if (err > 0) {
      breakpoint();
      Token *t;
      da_for(t, par->toks) {
        fprintf(stderr, "token #%zu: %s,", i_t+1, token_label(t));
	if (i_t == par->cur)
          fprintf(stderr, " // current\n");
	else
          fprintf(stderr, "\n");
      }
      return err;
    } else if (err < 0)
      break;

    da_add(&pro->code, node);
    Parser_advance(par);
  }

  return 0;
}

int Program_run(Program *pro) {
  Expr **expr;
  int value;

  value = 0;
  da_for(expr, &pro->code) {
    value = eval(*expr, pro->sco);
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

Program Program_setup(Scope *sco, Parser *par) {
  Program pro = {0};
  pro.toks = par->toks;
  pro.sco = sco;
  pro.par = par;
  return pro;
}

#endif // RAIZ_PROGRAM_C
