#ifndef RAIZ_RUNTIME_C
#define RAIZ_RUNTIME_C

int eval(Expr *e, Scope *s) {
  Symbol *sym, new_symbol;
  Scope *s_in;
  int value, ls, rs;
  char *ident;
  void *save;
  Expr **line;

  if (!e) {
    fprintf(stderr, "expression is null\n");
    return 0;
  }

  switch (e->kind) {
  case EXPR_LITERAL:
    return e->literal;
  case EXPR_UNARY:
    value = eval(e->unary.in, s);
    switch (e->unary.op.kind) {
    case TOKEN_MINUS:
      return -value;
    case TOKEN_BANG:
      return !value;
    case TOKEN_TILDE:
      return ~value;
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
      ident = e->binary.ls->ident;
      sym = Scope_search(s, ident, 0);
      if (sym == NULL) {
        fprintf(stderr, "undefined symbol '%s'\n", ident);
	return 0;
      }
      if (!sym->is_variable) {
        fprintf(stderr, "can't assign, '%s' is not a variable\n", ident);
	return 0;
      }
      sym->value = eval(e->binary.rs, s);
      return sym->value;
    }

    ls = eval(e->binary.ls, s);
    rs = eval(e->binary.rs, s);
    switch (e->binary.op.kind) {
    case TOKEN_PLUS:
      return ls + rs;
    case TOKEN_MINUS:
      return ls - rs;
    case TOKEN_STAR:
      return ls * rs;
    case TOKEN_SLASH:
      return ls / rs;
    case TOKEN_EQUAL_X2:
      return ls == rs;
    case TOKEN_BANG_EQUAL:
      return ls != rs;
    case TOKEN_PIPE:
      return ls | rs;
    case TOKEN_PIPE_X2:
      return ls || rs;
    case TOKEN_AMPER:
      return ls & rs;
    case TOKEN_AMPER_X2:
      return ls && rs;
    case TOKEN_HAT:
      return ls ^ rs;
    case TOKEN_LESS:
      return ls < rs;
    case TOKEN_LESS_EQUAL:
      return ls <= rs;
    case TOKEN_LESS_X2:
      return ls << rs;
    case TOKEN_GREAT:
      return ls > rs;
    case TOKEN_GREAT_EQUAL:
      return ls >= rs;
    case TOKEN_GREAT_X2:
      return ls >> rs;
    default:
      PANIC("invalid binary operator (token %s)\n", token_label(&e->binary.op));
    }
  case EXPR_GROUP:
    return eval(e->group.in, s);
  case EXPR_IDENT:
    sym = Scope_search(s, e->ident, 0);

    if (sym == NULL) {
      fprintf(stderr, "symbol '%s' not found\n", e->ident);
      return 0;
    }

    return sym->value;
  case EXPR_DECL:
    save = s->parent;
    s->parent = NULL;
    sym = Scope_search(s, e->decl.ident, 0);
    if (sym != NULL) {
      // already declared in this scope
      fprintf(stderr, "symbol '%s' already declared in this scope\n",
              e->decl.ident);
      return 0;
    }
    s->parent = (Scope*)save;

    value = e->decl.value != NULL ? eval(e->decl.value, s) : 0;
    new_symbol.value = value;
    new_symbol.is_variable = e->decl.kind == TOKEN_VAR;
    strcpy(new_symbol.ident, e->decl.ident);
    da_add(&s->symbols, new_symbol);

    return new_symbol.value;
  case EXPR_BLOCK:
    s_in = Scope_new(s);
    da_for(line, &e->block) {
      value = eval(*line, s_in);
    }
    free(s_in);
    s->inner = NULL;
    return value;
  }
  return 0;
}

#endif // RAIZ_RUNTIME_C
