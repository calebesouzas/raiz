#ifndef RAIZ_PROGRAM_C
#define RAIZ_PROGRAM_C

void breakpoint(void) {
  for (int i = 0; i < 3; i++);
}

int Program_build(Program *pro) {
  int err;
  Token *tok, *peeked;
  Expr *node;
  Parser *par;

  par = pro->par;

  while ((tok = Parser_cur(par))->kind != TOKEN_EOF) {
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

#undef add
#define add(code, token) da_add(errs, ((SemanticError){(code), (token)}))
void Expr_check(Expr *expr, SemanticError_A *errs, Scope *sco) {
  if (!expr)
    return;

  Symbol *sym;
  Token *ident;
  Scope *inner;
  void *save;

  switch (expr->kind) {
  case EXPR_LITERAL:
  case EXPR_UNARY:
    break; // nothing to check?
  case EXPR_BINARY:
    if (expr->binary.op->kind == TOKEN_EQUAL) {
      if (expr->binary.ls->kind != EXPR_IDENT) {
        add(ERR_SEM_ASSIGN_TO_RVALUE, NULL); // from where do I get the token?
      }
      ident = expr->binary.ls->ident;
      sym = Scope_search(sco, ident->source, ident->len);
      if (sym == NULL) {
        add(ERR_SEM_UNDEFINED_SYMBOL, ident);
      }
      if (!sym->is_variable) {
        add(ERR_SEM_ASSIGN_TO_VAL, ident);
      }
    }
    Expr_check(expr->binary.ls, errs, sco);
    Expr_check(expr->binary.rs, errs, sco);
    break;
  case EXPR_GROUP:
    Expr_check(expr->group.in, errs, sco);
    break;
  case EXPR_IDENT:
    sym = Scope_search(sco, expr->ident->source, expr->ident->len);

    if (sym == NULL) {
      add(ERR_SEM_UNDEFINED_SYMBOL, expr->ident);
    }
    break;
  case EXPR_DECL:
    ident = expr->decl.ident;
    save = sco->parent;
    sco->parent = NULL;
    sym = Scope_search(sco, ident->source, ident->len);
    if (sym != NULL) {
      add(ERR_SEM_ALREADY_DECLARED_SYMBOL, ident);
    }
    sco->parent = (Scope*)save;

    if (expr->decl.value)
      Expr_check(expr->decl.value, errs, sco);

    Symbol new_symbol = {0};
    new_symbol.is_variable = expr->decl.tok->kind == TOKEN_VAR;
    strncpy(new_symbol.ident, expr->decl.ident->source, expr->decl.ident->len);
    new_symbol.ident[expr->decl.ident->len] = '\0';

    da_add(&sco->symbols, new_symbol);

    break;
  case EXPR_BLOCK:
    inner = Scope_new(sco);
    Expr **line;
    da_for(line, &expr->block) {
      Expr_check(*line, errs, sco);
    }
    free(inner);
    sco->inner = NULL;
    break;
  }
}

void Program_check(Program *pro, SemanticError_A *errs, size_t max_errs) {
  Expr **expr;
  Scope *sco = Scope_copy(pro->sco);
  da_for(expr, &pro->code) {
    Expr_check(*expr, errs, pro->sco);
    if (errs->len > max_errs)
      return;
  }
  Scope_free(sco);
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

void Program_free(Program *pro) {
  Expr **expr;

  da_for(expr, &pro->code) {
    Expr_free(*expr);
  }

  Scope_free(pro->sco);
  da_free(pro->toks);
}

#endif // RAIZ_PROGRAM_C
