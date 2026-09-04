#ifndef RAIZ_RUNTIME_C
#define RAIZ_RUNTIME_C

#define READ_BUF_CAP 1024

Value *eval_lvalue(Expr *e, Scope *s) {
  if (e->kind == EXPR_UNARY && e->unary.op->kind == TOKEN_STAR) {
    return eval_lvalue(e->unary.in, s);
  }
  switch (e->kind) {
  case EXPR_GROUP:
    return eval_lvalue(e->group.in, s);
  case EXPR_IDENT: {
    Symbol *sym = Scope_search_until_global(s, token_sv(e->ident));
    return &sym->val;
  } break;
  case EXPR_PARENT: {
    Scope *target = s;
    uint32_t level = e->parent.level;
    Token *identifier = e->parent.ident;

    for (uint32_t i = 0; target && i < level; i++)
      target = target->parent;

    Symbol *sym = Scope_search_until_global(target, token_sv(identifier));
    return &sym->val;
  } break;
  default: UNREACHABLE("maybe not lvalue? (expr kind %d)", e->kind);
  }
  return NULL;
}

EvalResult eval(Expr *e, Scope *s) {
  Symbol *sym, new_symbol;
  Scope *s_in, *target;
  uint64_t ls, rs, cond;
  Value value, lsv, rsv, *value_p;
  char *ident;
  void *save;
  Expr **line;
  EvalResult res = {0};
  EvalResult other = {0};

  switch (e->kind) {
  case EXPR_LITERAL:
    res.value = *Value_alloc(e->literal->literal);
    break;
  case EXPR_UNARY:
    value_p = eval_lvalue(e->unary.in, s);
    switch (e->unary.op->kind) {
    case TOKEN_MINUS:
      res.value.data = -value_p->data;
      break;
    case TOKEN_BANG:
      res.value.data = !value_p->data;
      break;
    case TOKEN_TILDE:
      res.value.data = ~value_p->data;
      break;
    case TOKEN_AMPER:
      res.value.data = (uintptr_t)value_p;
      break;
    case TOKEN_STAR:
      res.value.data = value_p->data;
      break;
    default:
      PANIC("invalid unary operator (token %s)\n", token_label(e->binary.op));
    }
    res.value.type = value_p->type;
    break;
  case EXPR_BINARY:
    if (e->binary.op->kind == TOKEN_EQUAL) {
      ident = e->binary.ls->ident->lexeme;
      Value *valp = eval_lvalue(e->binary.ls, s);
      *valp = eval(e->binary.rs, s).value;
      res.value = *valp;
      return res;
    }

    lsv = eval(e->binary.ls, s).value;
    ls = lsv.data;

    rsv = eval(e->binary.rs, s).value;
    rs = rsv.data;

    res.value.type = lsv.type;

    switch (e->binary.op->kind) {
    case TOKEN_PLUS:
      res.value.data = ls + rs;
      break;
    case TOKEN_MINUS:
      res.value.data = ls - rs;
      break;
    case TOKEN_STAR:
      res.value.data = ls * rs;
      break;
    case TOKEN_SLASH:
      res.value.data = ls / rs;
      break;
    case TOKEN_PERCENT:
      res.value.data = ls % rs;
      break;
    case TOKEN_EQUAL_X2:
      res.value.data = ls == rs;
      res.value.type = &g_TYPE_bool;
      break;
    case TOKEN_BANG_EQUAL:
      res.value.data = ls != rs;
      res.value.type = &g_TYPE_bool;
      break;
    case TOKEN_PIPE:
      res.value.data = ls | rs;
      break;
    case TOKEN_PIPE_X2:
      res.value.data = ls || rs;
      res.value.type = &g_TYPE_bool;
      break;
    case TOKEN_AMPER:
      res.value.data = ls & rs;
      break;
    case TOKEN_AMPER_X2:
      res.value.data = ls && rs;
      res.value.type = &g_TYPE_bool;
      break;
    case TOKEN_HAT:
      res.value.data = ls ^ rs;
      break;
    case TOKEN_LESS:
      res.value.data = ls < rs;
      res.value.type = &g_TYPE_bool;
      break;
    case TOKEN_LESS_EQUAL:
      res.value.data = ls <= rs;
      res.value.type = &g_TYPE_bool;
      break;
    case TOKEN_LESS_X2:
      res.value.data = ls << rs;
      break;
    case TOKEN_GREAT:
      res.value.data = ls > rs;
      break;
    case TOKEN_GREAT_EQUAL:
      res.value.data = ls >= rs;
      res.value.type = &g_TYPE_bool;
      break;
    case TOKEN_GREAT_X2:
      res.value.data = ls >> rs;
      break;
    default:
      PANIC("invalid binary operator (token %s)\n", token_label(e->binary.op));
    }
    break;
  case EXPR_GROUP:
    res.value = eval(e->group.in, s).value;
    break;
  case EXPR_IDENT:
    sym = Scope_search_until_global(s, token_sv(e->ident));
    res.value = sym->val;
    break;
  case EXPR_DECL:
    sym = Scope_search_single_level(s, token_sv(e->decl.ident));

    value = e->decl.value != NULL ? eval(e->decl.value, s).value : (Value){0};
    switch (e->decl.kind->kind) {
    case TOKEN_VAR:
      new_symbol.kind = SYM_VAR;
      break;
    case TOKEN_VAL:
      new_symbol.kind = SYM_VAL;
      break;
    default:
      PANIC("unhandled declaration token: %s\n", token_label(e->decl.kind));
      break;
    }
    new_symbol.val = value;
    new_symbol.ident = token_sv(e->decl.ident);
    Scope_insert(s, new_symbol);

    res.value = new_symbol.val;
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

    sym = Scope_search_until_global(target, token_sv(identifier));
    res.value = sym->val;
    break;
  case EXPR_IF:
    cond = eval(e->if_node.cond, s).value.data;
    if (cond) {
      res = eval(e->if_node.then_branch, s);
    } else if (e->if_node.else_branch) {
      res = eval(e->if_node.else_branch, s);
    }
    break;
  case EXPR_WHILE:
    while ((cond = eval(e->while_node.cond, s).value.data)) {
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
  case EXPR_PRINT:
    res.value = eval(e->print.value, s).value;
    switch (res.value.type->kind) {
    case TYPE_int:
      printf("%d", (int)res.value.data);
      break;
    case TYPE_char:
      printf("%c", (char)res.value.data);
      break;
    case TYPE_bool:
      printf("%s", res.value.data ? "true" : "false");
      break;
    case TYPE_byte:
      printf("%02x", (unsigned char)res.value.data);
      break;
    }
    break;
  case EXPR_READ: {
    TODO("implement EXPR_READ runtime");
#if 0
    char buf[READ_BUF_CAP];
    fgets(buf, READ_BUF_CAP, stdin);
    res.value.data = sp_save(buf);
    res.value.type = &g_TYPE_string;
#endif
  } break;
  }
  return res;
}

#endif // RAIZ_RUNTIME_C
