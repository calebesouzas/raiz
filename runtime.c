#ifndef RAIZ_RUNTIME_C
#define RAIZ_RUNTIME_C

int eval(Expr *e, Symbol_A *symbols) {
  if (!e) {
    fprintf(stderr, "expression is null\n");
    return 0;
  }

  switch (e->kind) {
  case EXPR_LITERAL:
    return e->literal;
  case EXPR_UNARY:
    switch (e->unary.op.kind) {
    case TOKEN_MINUS:
      return -eval(e->unary.in, symbols);
    }
  case EXPR_BINARY: {
    int ls = eval(e->binary.ls, symbols);
    int rs = eval(e->binary.rs, symbols);
    switch (e->binary.op.kind) {
    case TOKEN_PLUS:
      return ls + rs;
    case TOKEN_MINUS:
      return ls - rs;
    case TOKEN_STAR:
      return ls * rs;
    case TOKEN_SLASH:
      return ls / rs;
    }
  } break;
  case EXPR_GROUP:
    return eval(e->group.in, symbols);
  case EXPR_IDENT: {
    Symbol *sym;
    da_for(sym, symbols) {
      if (strcmp(e->ident, sym->ident) == 0) {
        return symbols->dat[i_sym].value;
      }
    }
    fprintf(stderr, "symbol '%s' not found\n", e->ident);
  } break;
  }
  return 0;
}

#endif // RAIZ_RUNTIME_C
