#ifndef RAIZ_RUNTIME_C
#define RAIZ_RUNTIME_C

#define READ_BUF_CAP 1024

EvalResult eval(Expr *e, Scope *s) {
  Symbol *sym, new_symbol;
  Scope *s_in, *target;
  uint64_t ls, rs, cond;
  Value value;
  char *ident;
  void *save;
  Expr **line;
  EvalResult res = {0};
  EvalResult other = {0};

  switch (e->kind) {
  case EXPR_LITERAL:
    res.value = e->literal->literal;
    break;
  case EXPR_UNARY:
    value = eval(e->unary.in, s).value;
    switch (e->unary.op->kind) {
    case TOKEN_MINUS:
      res.value.data = -value.data;
      break;
    case TOKEN_BANG:
      res.value.data = !value.data;
      break;
    case TOKEN_TILDE:
      res.value.data = ~value.data;
      break;
    default:
      PANIC("invalid unary operator (token %s)\n", token_label(e->binary.op));
    }
    res.value.type = value.type;
    break;
  case EXPR_BINARY:
    if (e->binary.op->kind == TOKEN_EQUAL) {
      ident = e->binary.ls->ident->lexeme;
      sym = Scope_search_until_global(s, ident, e->binary.ls->ident->len);
      sym->value = eval(e->binary.rs, s).value;
      res.value = sym->value;
      return res;
    }

    Value lsv = eval(e->binary.ls, s).value;
    ls = lsv.data;

    Value rsv = eval(e->binary.rs, s).value;
    rs = rsv.data;

    res.value.type = lsv.type;

    if (res.value.type == &g_TYPE_string) {
      sb_t *lsb = sp_get((size_t) ls);
      sb_t *rsb = sp_get((size_t) rs);
      switch (e->binary.op->kind) {
      case TOKEN_PLUS:
        sb_push_sv(lsb, *(sv_t*)rsb);
        res.value.data = ls;
        break;
      case TOKEN_EQUAL_X2:
        res.value.data = lsb->len == rsb->len
          && strncmp(lsb->ptr, rsb->ptr, lsb->len) == 0;
        res.value.type = &g_TYPE_bool;
        break;
      case TOKEN_BANG_EQUAL:
        res.value.data = lsb->len != rsb->len
          && strncmp(lsb->ptr, rsb->ptr, lsb->len) != 0;
        res.value.type = &g_TYPE_bool;
        break;
      case TOKEN_AMPER_X2:
        res.value.data = lsb->len > 0 && rsb->len > 0;
        res.value.type = &g_TYPE_bool;
        break;
      case TOKEN_PIPE_X2:
        res.value.data = lsb->len > 0 || rsb->len > 0;
        res.value.type = &g_TYPE_bool;
        break;
      }
      return res;
    }

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
    case TOKEN_EQUAL_X2:
      res.value.data = ls == rs;
      break;
    case TOKEN_BANG_EQUAL:
      res.value.data = ls != rs;
      break;
    case TOKEN_PIPE:
      res.value.data = ls | rs;
      break;
    case TOKEN_PIPE_X2:
      res.value.data = ls || rs;
      break;
    case TOKEN_AMPER:
      res.value.data = ls & rs;
      break;
    case TOKEN_AMPER_X2:
      res.value.data = ls && rs;
      break;
    case TOKEN_HAT:
      res.value.data = ls ^ rs;
      break;
    case TOKEN_LESS:
      res.value.data = ls < rs;
      break;
    case TOKEN_LESS_EQUAL:
      res.value.data = ls <= rs;
      break;
    case TOKEN_LESS_X2:
      res.value.data = ls << rs;
      break;
    case TOKEN_GREAT:
      res.value.data = ls > rs;
      break;
    case TOKEN_GREAT_EQUAL:
      res.value.data = ls >= rs;
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
    sym = Scope_search_until_global(s, e->ident->lexeme, e->ident->len);
    res.value = sym->value;
    break;
  case EXPR_DECL:
    sym = Scope_search_single_level(
              s, e->decl.ident->lexeme, e->decl.ident->len);

    value = e->decl.value != NULL ? eval(e->decl.value, s).value : (Value){0};
    value.type = e->decl.type;
    new_symbol.value = value;
    new_symbol.is_variable = e->decl.tok->kind == TOKEN_VAR;
    new_symbol.ident = e->decl.ident;
    Scope_insert(s, new_symbol);

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
      case TYPE_string: {
        sb_t *sb = sp_get((size_t) res.value.data);
        printf("%.*s", size_t_int(sb->len), sb->ptr);
      } break;
    }
    break;
  case EXPR_READ: {
    char buf[READ_BUF_CAP];
    fgets(buf, READ_BUF_CAP, stdin);
    res.value.data = sp_save(buf);
    res.value.type = &g_TYPE_string;
  } break;
  }
  return res;
}

#endif // RAIZ_RUNTIME_C
