#ifndef RAIZ_RUNTIME_C
#define RAIZ_RUNTIME_C

EvalResult eval(Expr *e, Scope *s) {
  Symbol *sym, new_symbol;
  Scope *s_in, *target;
  int value, ls, rs, cond;
  char *ident;
  void *save;
  Expr **line;
  EvalResult res = {0};
  EvalResult other = {0};

  switch (e->kind) {
  case EXPR_LITERAL:
    res.value = e->literal->value;
    break;
  case EXPR_UNARY:
    value = eval(e->unary.in, s).value;
    switch (e->unary.op->kind) {
    case TOKEN_MINUS:
      res.value = -value;
      break;
    case TOKEN_BANG:
      res.value = !value;
      break;
    case TOKEN_TILDE:
      res.value = ~value;
      break;
    default:
      PANIC("invalid unary operator (token %s)\n", token_label(e->binary.op));
    }
    break;
  case EXPR_BINARY:
    if (e->binary.op->kind == TOKEN_EQUAL) {
      ident = e->binary.ls->ident->lexeme;
      sym = Scope_search_until_global(s, ident, e->binary.ls->ident->len);
      sym->value = eval(e->binary.rs, s).value;
      res.value = sym->value;
      return res;
    }

    ls = eval(e->binary.ls, s).value;
    rs = eval(e->binary.rs, s).value;
    switch (e->binary.op->kind) {
    case TOKEN_PLUS:
      res.value = ls + rs;
      break;
    case TOKEN_MINUS:
      res.value = ls - rs;
      break;
    case TOKEN_STAR:
      res.value = ls * rs;
      break;
    case TOKEN_SLASH:
      res.value = ls / rs;
      break;
    case TOKEN_EQUAL_X2:
      res.value = ls == rs;
      break;
    case TOKEN_BANG_EQUAL:
      res.value = ls != rs;
      break;
    case TOKEN_PIPE:
      res.value = ls | rs;
      break;
    case TOKEN_PIPE_X2:
      res.value = ls || rs;
      break;
    case TOKEN_AMPER:
      res.value = ls & rs;
      break;
    case TOKEN_AMPER_X2:
      res.value = ls && rs;
      break;
    case TOKEN_HAT:
      res.value = ls ^ rs;
      break;
    case TOKEN_LESS:
      res.value = ls < rs;
      break;
    case TOKEN_LESS_EQUAL:
      res.value = ls <= rs;
      break;
    case TOKEN_LESS_X2:
      res.value = ls << rs;
      break;
    case TOKEN_GREAT:
      res.value = ls > rs;
      break;
    case TOKEN_GREAT_EQUAL:
      res.value = ls >= rs;
      break;
    case TOKEN_GREAT_X2:
      res.value = ls >> rs;
      break;
    default:
      PANIC("invalid binary operator (token %s)\n", token_label(e->binary.op));
    }
    break;
  case EXPR_GROUP:
    res.value = eval(e->group.in, s).value;
    break;
  case EXPR_IDENT:
    sym = Scope_search_until_global(s, e->ident->lexeme, e->ident->len);
    res.value = sym->value;
    break;
  case EXPR_DECL:
    sym = Scope_search_single_level(
              s, e->decl.ident->lexeme, e->decl.ident->len);

    value = e->decl.value != NULL ? eval(e->decl.value, s).value : 0;
    new_symbol.value = value;
    new_symbol.is_variable = e->decl.tok->kind == TOKEN_VAR;
    strncpy(new_symbol.ident, e->decl.ident->lexeme, e->decl.ident->len);
    new_symbol.ident[e->decl.ident->len] = '\0';
    da_add(&s->symbols, new_symbol);

    res.value = new_symbol.value;
    break;
  case EXPR_BLOCK:
    s_in = Scope_new(s);
    da_for(line, &e->block) {
      res = eval(*line, s_in);
      if (res.sig == SIGNAL_BREAK)
        break;
    }
    free(s_in);
    s->inner = NULL;
    break;
  case EXPR_PARENT:
    target = s;
    uint32_t level = e->parent.level;
    Token *identifier = e->parent.ident;

    for (uint32_t i = 0; target && i < level; i++)
      target = target->parent;

    sym = Scope_search_until_global(
              target, identifier->lexeme, identifier->len);
    res.value = sym->value;
    break;
  case EXPR_IF:
    cond = eval(e->if_node.cond, s).value;
    if (cond) {
      res = eval(e->if_node.then_branch, s);
    } else if (e->if_node.else_branch) {
      res = eval(e->if_node.else_branch, s);
    }
    break;
  case EXPR_WHILE:
    while ((cond = eval(e->while_node.cond, s).value)) {
      if (res.sig == SIGNAL_BREAK)
        break;
      if (res.sig == SIGNAL_CONTINUE)
        continue;

      res = eval(e->while_node.body, s);
    }
    if (res.sig == SIGNAL_BREAK && e->while_node.else_branch) {
      res = eval(e->while_node.else_branch, s);
    } else if (e->while_node.then_branch) {
      res = eval(e->while_node.then_branch, s);
    }
    res.sig = SIGNAL_NONE;
    break;
  case EXPR_BREAK:
    res.sig = SIGNAL_BREAK;
    break;
  case EXPR_CONTINUE:
    res.sig = SIGNAL_CONTINUE;
    break;
  }
  return res;
}

#endif // RAIZ_RUNTIME_C
