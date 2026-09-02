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

Value Program_run(Program *pro) {
  Expr **expr;
  Value value = {0};

  da_for(expr, &pro->code) {
    value = eval(*expr, pro->sco).value;
  }

  return value;
}

#undef add
#define add(code, ...)\
  da_add(errs, ((SemanticError){(code), __VA_ARGS__}))

#define ctx_success(...)\
  do {\
    ctx = (SemanticContext){\
      .ok = true,\
      __VA_ARGS__\
    };\
    return ctx;\
  } while (0)

#define ctx_success_with(other)\
  do {\
    ctx = (other);\
    ctx.ok = true;\
    return ctx;\
  } while (0)

#define ctx_err(code, ...)\
  do {\
    ctx.ok = false;\
    add(code, __VA_ARGS__);\
    return ctx;\
  } while (0)

#define ctx_check(c) if (!(c).ok) return c

void ctx_propagate_down(SemanticContext *source, SemanticContext *target) {
  if (source)
    target->data = source->data;
}

SemanticContext
Expr_check(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  SemanticContext ctx, ctx_left, ctx_right, ctx_in;

  ctx_propagate_down(out, &ctx);

  assert(expr != NULL);

  Symbol *sym;
  Token *ident;
  Scope *inner;
  void *save;

  switch (expr->kind) {
  case EXPR_LITERAL:
    ctx_success(.type = expr->literal->literal.type, .is_constant = true);
  case EXPR_UNARY:
    break;
  case EXPR_BREAK:
  case EXPR_CONTINUE:
    if (!ctx.data.inside_loop)
      ctx_err(ERR_SEM_LOOP_KEYWORD_OUTSIDE_LOOP, .token = expr->token);

    ctx_success();
  case EXPR_READ:
    TODO("check EXPR_READ");
  case EXPR_BINARY:
    ctx_left = Expr_check(expr->binary.ls, errs, sco, &ctx);
    ctx_check(ctx_left);

    if (expr->binary.op->kind == TOKEN_EQUAL) {
      if (!ctx_left.is_lvalue) {
        ctx_err(ERR_SEM_ASSIGN_TO_RVALUE, .expr = expr);
      } else if (!ctx_left.is_variable) {
        ctx_err(ERR_SEM_ASSIGN_TO_VAL, .expr = expr);
      }
    }

    ctx_right = Expr_check(expr->binary.rs, errs, sco, &ctx);
    ctx_check(ctx_right);

    if (ctx_left.type != ctx_right.type) {
      ctx_err(ERR_SEM_INCOMPATIBLE_TYPES,
        .expr = expr,
        .type = {ctx_left.type, ctx_right.type});
    }

    ctx_success_with(ctx_left);
  case EXPR_GROUP:
    ctx_in = Expr_check(expr->group.in, errs, sco, &ctx);
    ctx_check(ctx_in);

    ctx_success_with(ctx_in);
  case EXPR_DECL:
    ident = expr->decl.ident;
    sym = Scope_search_single_level(sco, token_sv(ident),
      .ignore_kind = SYM_TYPE);
    if (sym != NULL) {
      ctx_err(ERR_SEM_ALREADY_DECLARED_SYMBOL, .token = ident);
    }

    const Type *type = Type_find(sco, expr->decl.type);
    if (!type) {
      ctx_err(ERR_SEM_UNDEFINED_TYPE,
        .expr = expr,
        .type_pattern = expr->decl.type);
    }

    if (expr->decl.value) {
      ctx_in = Expr_check(expr->decl.value, errs, sco, &ctx);
      ctx_check(ctx_in);

      if (type != ctx_in.type) {
        ctx_err(ERR_SEM_INCOMPATIBLE_TYPES,
          .expr = expr,
          .type = {type, ctx_in.type});
      }
    }

    Symbol new_symbol = {0};
    new_symbol.ident = token_sv(ident);
    switch (expr->decl.kind->kind) {
    case TOKEN_VAR:
      new_symbol.kind = SYM_VAR;
      new_symbol.var.type = type;
      break;
    case TOKEN_VAL:
      new_symbol.kind = SYM_VAL;
      new_symbol.val.type = type;
      break;
    default:
      PANIC("unhandled declaration token: %s\n", token_label(expr->decl.kind));
      break;
    }

    Scope_insert(sco, new_symbol);

    // ctx.is_lvalue = true; // maybe set as L-value?
    ctx_success(.type = type);
  case EXPR_BLOCK:
    inner = Scope_new(sco);
    Expr **line;
    da_for(line, &expr->block) {
      ctx_in = Expr_check(*line, errs, inner, &ctx);
    }
    free(inner);
    sco->inner = NULL;

    ctx_success_with(ctx_in);
  case EXPR_IDENT:
    sym = Scope_search_until_global(sco, token_sv(expr->ident));

    if (sym == NULL) {
      ctx_err(ERR_SEM_UNDEFINED_SYMBOL, .token = expr->ident);
    }

    ctx_success(.type = sym->val.type = sym->val.type,
      .is_lvalue = true, .is_variable = sym->kind == SYM_VAR);
  case EXPR_PARENT:
    ident = expr->parent.ident;
    Scope *target = sco;
    uint32_t level = expr->parent.level;

    size_t count;
    for (count = 0; target && count < level; count++)
      target = target->parent;

    sym = Scope_search_until_global(target, token_sv(ident));
    if (sym == NULL) {
      ctx_err(ERR_SEM_UNDEFINED_SYMBOL, .token = ident, .count = count);
    }

    ctx_success(.is_lvalue = true,
      .is_variable = sym->kind == SYM_VAR,
      .is_constant = false);
  case EXPR_IF:
    if (expr->if_node.then_branch->kind == EXPR_DECL) {
      ctx_err(ERR_SEM_DECL_AFTER_IF_ELSE, .expr = expr->if_node.then_branch);
    } else if (expr->if_node.else_branch
        && expr->if_node.else_branch->kind == EXPR_DECL) {
      ctx_err(ERR_SEM_DECL_AFTER_IF_ELSE, .expr = expr->if_node.else_branch);
    }

    ctx_in = Expr_check(expr->if_node.cond, errs, sco, &ctx);
    ctx_check(ctx_in);

    ctx_in = Expr_check(expr->if_node.then_branch, errs, sco, &ctx);
    ctx_check(ctx_in);

    if (expr->if_node.else_branch) {
      const Type *then_type = ctx_in.type;
      ctx_in = Expr_check(expr->if_node.else_branch, errs, sco, &ctx);
      ctx_check(ctx_in);

      if (then_type != ctx_in.type) {
        ctx_err(ERR_SEM_INCOMPATIBLE_TYPES,
          .expr = expr,
          .type = {then_type, ctx_in.type});
      }
    }

    ctx_success(.type = ctx_in.type);
  case EXPR_WHILE:
    ctx.data.inside_loop = true;

    if (expr->while_node.body->kind == EXPR_DECL) {
      ctx_err(ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE,
        .expr = expr->while_node.body);
    } else if (expr->while_node.then_branch
        && expr->while_node.then_branch->kind == EXPR_DECL) {
      ctx_err(ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE,
        .expr = expr->while_node.then_branch);
    } else if (expr->while_node.else_branch
        && expr->while_node.else_branch->kind == EXPR_DECL) {
      ctx_err(ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE,
        .expr = expr->while_node.else_branch);
    }

    ctx_in = Expr_check(expr->while_node.cond, errs, sco, &ctx);
    ctx_check(ctx_in);

    ctx_in = Expr_check(expr->while_node.body, errs, sco, &ctx);
    ctx_check(ctx_in);

    if (expr->while_node.then_branch) {
      ctx_in = Expr_check(expr->while_node.then_branch, errs, sco, &ctx);
      ctx_check(ctx_in);
    }

    if (expr->while_node.else_branch) {
      ctx_in = Expr_check(expr->while_node.else_branch, errs, sco, &ctx);
      ctx_check(ctx_in);
    }

    ctx_success();
  case EXPR_PRINT:
    ctx_in = Expr_check(expr->print.value, errs, sco, &ctx);
    ctx_check(ctx_in);

    ctx_success(.type = ctx_in.type);
  }
  return ctx;
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
