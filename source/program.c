#ifndef RAIZ_PROGRAM_C
#define RAIZ_PROGRAM_C

int Program_build(Program *pro) {
  return parse_program(pro->par);
}

Value Program_run(Program *pro) {
  Expr **expr;
  Value value;

  da_for(expr, &pro->code) {
    value = eval(*expr, pro->sco).value;
  }

  return value;
}

void Program_check(Program *pro, SemanticError_A *errs, size_t max_errs) {
  Expr **expr;
  Scope *sco = Scope_copy(pro->sco);
  da_for(expr, &pro->code) {
    (void) Expr_check(*expr, errs, sco, NULL);
    if (errs->len > max_errs)
      return;
  }
  Scope_free(sco);
}

Program Program_setup(Scope *sco, Parser *par) {
  Program pro = {0};
  pro.toks = par->toks;
  pro.sco = sco;
  pro.par = par;
  return pro;
}

void Program_free(Program *pro) {
  Expr **expr;

  da_for(expr, &pro->code) {
    Expr_free(*expr);
  }

  Scope_free(pro->sco);
  da_free(pro->toks);
}

#endif // RAIZ_PROGRAM_C
