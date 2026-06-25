#ifndef RAIZ_RUNTIME_C
#define RAIZ_RUNTIME_C

int eval(Expr *e, Symbol_A *symbols) {
  Symbol *sym, new_symbol;
  int value, ls, rs;

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
    default:
      PANIC("invalid unary operator (token %s)\n", token_label(&e->binary.op));
    }
    break;
  case EXPR_BINARY:
    if (e->binary.op.kind == TOKEN_EQUAL) {
      if (e->binary.ls->kind != EXPR_IDENT) {
        fprintf(stderr, "can only assign to indentifiers\n");
	return 0;
      }
      char *ident = e->binary.ls->ident;
      da_for(sym, symbols) {
        if (strcmp(ident, sym->ident) != 0)
          continue;
	if (!sym->is_variable) {
          fprintf(stderr, "can't assign, '%s' is not a variable\n", ident);
	  return 0;
	}
	sym->value = eval(e->binary.rs, symbols);
	return sym->value;
      }
    }

    ls = eval(e->binary.ls, symbols);
    rs = eval(e->binary.rs, symbols);
    switch (e->binary.op.kind) {
    case TOKEN_PLUS:
      return ls + rs;
    case TOKEN_MINUS:
      return ls - rs;
    case TOKEN_STAR:
      return ls * rs;
    case TOKEN_SLASH:
      return ls / rs;
    default:
      PANIC("invalid binary operator (token %s)\n", token_label(&e->binary.op));
    }
  case EXPR_GROUP:
    return eval(e->group.in, symbols);
  case EXPR_IDENT: {
    da_for(sym, symbols) {
      if (strcmp(e->ident, sym->ident) == 0) {
        return symbols->dat[i_sym].value;
      }
    }
    fprintf(stderr, "symbol '%s' not found\n", e->ident);
  } break;
  case EXPR_DECL:
    value = e->decl.value != NULL ? eval(e->decl.value, symbols) : 0;
    da_for(sym, symbols) {
      if (!sym->is_variable) {
        fprintf(stderr, "attempt to assign to value %s\n", sym->ident);
        return 0;
      }
      if (strcmp(sym->ident, e->decl.ident) == 0) {
        sym->value = value;
        return sym->value;
      }
    }

    new_symbol.value = value;
    new_symbol.is_variable = e->decl.kind == TOKEN_VAR;
    strcpy(new_symbol.ident, e->decl.ident);
    da_add(symbols, new_symbol);

    return new_symbol.value;
  default: UNREACHABLE("expression kind (%d)\n", e->kind);
  }
  return 0;
}

#endif // RAIZ_RUNTIME_C
