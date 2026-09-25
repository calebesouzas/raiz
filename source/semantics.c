#undef add
#define add(code, ...)\
  da_add(errs, ((SemanticError){(code), __VA_ARGS__}))

#define ctx_success(...)\
  return (SemanticContext){__VA_ARGS__}

#define ctx_success_with(other)\
  return (other);

#define ctx_err_with(ctx, code, ...)\
  do {\
    (ctx).err = true;\
    add(code, __VA_ARGS__);\
    return (ctx);\
  } while (0)

#define ctx_err(code, ...)\
  do {\
    add(code, __VA_ARGS__);\
    return (SemanticContext){.err = true};\
  } while (0)

#define ctx_check(c) if ((c).err) return (c);

void ctx_propagate_down(SemanticContext *source, SemanticContext *target) {
  if (source)
    target->data = source->data;
}

SemanticContext Expr_check(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
);

SemanticContext Expr_check_literal(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_LITERAL);

  SemanticContext ctx;
  ctx_success(.type = expr->literal->literal.type, .is_constant = true);
}

SemanticContext Expr_check_binary(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_BINARY);

  SemanticContext ctx_left = Expr_check(expr->binary.ls, errs, sco, out);
  ctx_check(ctx_left);

  if (expr->binary.op->kind == TOKEN_EQUAL) {
    if (!ctx_left.is_lvalue) {
      ctx_err_with(ctx_left, ERR_SEM_ASSIGN_TO_RVALUE, .expr = expr);
    }
  }

  SemanticContext ctx_right = Expr_check(expr->binary.rs, errs, sco, out);
  ctx_check(ctx_right);

  if (ctx_left.type != ctx_right.type) {
    ctx_err(ERR_SEM_INCOMPATIBLE_TYPES,
      .expr = expr,
      .type = {ctx_left.type, ctx_right.type});
  }

  ctx_success_with(ctx_left);
}

SemanticContext Expr_check_unary(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_UNARY);

  SemanticContext ctx_in = Expr_check(expr->unary.in, errs, sco, out);
  ctx_check(ctx_in);

  Type *type = ctx_in.type;

  switch (expr->unary.op->kind) {
  case TOKEN_MINUS:
    break;
  case TOKEN_TILDE:
    break;
  case TOKEN_BANG:
    type = &g_TYPE_bool;
    break;
  case TOKEN_AMPER:
    type = Type_find(sco, (TypePattern){
      .name = ctx_in.type->pattern.name,
      .ptr_count = ctx_in.type->pattern.ptr_count + 1});
    break;
  case TOKEN_STAR:
    if (ctx_in.type->pattern.ptr_count == 0) {
      ctx_err_with(ctx_in, ERR_SEM_DEREF_NON_POINTER,
        .token = expr->unary.op);
    }
    type = Type_find(sco, (TypePattern){
      .name = ctx_in.type->pattern.name,
      .ptr_count = ctx_in.type->pattern.ptr_count - 1});
    ctx_success(.type = type, .is_lvalue = true,
      .deref_count = ctx_in.deref_count + 1);
    break;
  default: UNREACHABLE("not an unary operator");
  }

  ctx_success(.type = type);
}

SemanticContext Expr_check_group(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_GROUP);

  SemanticContext ctx_in = Expr_check(expr->group.in, errs, sco, out);
  ctx_check(ctx_in);

  ctx_success_with(ctx_in);
}

SemanticContext Expr_check_ident(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_IDENT);

  Symbol *sym = Scope_search_until_global(sco, token_sv(expr->ident),
    .search_kind = SYM_VAR);

  if (sym == NULL) {
    ctx_err(ERR_SEM_UNDEFINED_SYMBOL, .token = expr->ident);
  }

  ctx_success(.type = sym->var.type,
    .is_lvalue = true);
}

SemanticContext Expr_check_decl(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_DECL);

  Token *ident = expr->decl.ident;
  Symbol *sym = Scope_search_single_level(sco, token_sv(ident),
    .ignore_kind = SYM_TYPE);

  if (sym != NULL) {
    ctx_err(ERR_SEM_ALREADY_DECLARED_SYMBOL, .token = ident);
  }

  Type *type = Type_find(sco, expr->decl.type);
  if (!type) {
    ctx_err(ERR_SEM_UNDEFINED_TYPE,
      .expr = expr,
      .type_pattern = expr->decl.type);
  }

  if (expr->decl.value) {
    SemanticContext ctx_in = Expr_check(expr->decl.value, errs, sco, out);
    ctx_check(ctx_in);

    if (!Type_equals(type, ctx_in.type)) {
      ctx_err(ERR_SEM_INCOMPATIBLE_TYPES,
        .expr = expr,
        .type = {type, ctx_in.type});
    }
  }

  Symbol new_symbol = {0};
  new_symbol.ident = token_sv(ident);
  new_symbol.kind = SYM_VAR;
  new_symbol.var.type = type;

  Scope_insert(sco, new_symbol);

  // ctx.is_lvalue = true; // maybe set as L-value?
  ctx_success(.type = type);
}

SemanticContext Expr_check_block(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_BLOCK);

  Scope *inner = Scope_new(sco);

  SemanticContext ctx_in = {0};
  da_iter(line, &expr->block) {
    ctx_in = Expr_check(*line, errs, inner, out);
  }
  free(inner);
  sco->inner = NULL;

  ctx_success_with(ctx_in);
}

SemanticContext Expr_check_parent(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_PARENT);

  Token *ident = expr->parent.ident;
  Scope *target = sco;
  uint32_t level = expr->parent.level;

  size_t count;
  for (count = 0; target && count < level; count++)
    target = target->parent;

  Symbol *sym = Scope_search_until_global(target, token_sv(ident),
    .search_kind = SYM_VAR);

  if (sym == NULL) {
    ctx_err(ERR_SEM_UNDEFINED_SYMBOL, .token = ident, .count = count);
  }

  ctx_success(.is_lvalue = true,
    .is_constant = false);
}

SemanticContext Expr_check_if(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_IF);

  SemanticContext ctx = {0};
  if (expr->if_node.then_branch->kind == EXPR_DECL) {
    ctx_err(ERR_SEM_DECL_AFTER_IF_ELSE, .expr = expr->if_node.then_branch);
  } else if (expr->if_node.else_branch
      && expr->if_node.else_branch->kind == EXPR_DECL) {
    ctx_err(ERR_SEM_DECL_AFTER_IF_ELSE, .expr = expr->if_node.else_branch);
  }

  SemanticContext ctx_in = Expr_check(expr->if_node.cond, errs, sco, out);
  ctx_check(ctx_in);

  ctx_in = Expr_check(expr->if_node.then_branch, errs, sco, out);
  ctx_check(ctx_in);

  if (expr->if_node.else_branch) {
    Type *then_type = ctx_in.type;
    ctx_in = Expr_check(expr->if_node.else_branch, errs, sco, out);
    ctx_check(ctx_in);

    if (then_type != ctx_in.type) {
      ctx_err(ERR_SEM_INCOMPATIBLE_TYPES,
        .expr = expr,
        .type = {then_type, ctx_in.type});
    }
  }

  ctx_success(.type = ctx_in.type);
}

SemanticContext Expr_check_while(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_WHILE);

  SemanticContext ctx = {0};
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

  SemanticContext ctx_in = Expr_check(expr->while_node.cond, errs, sco, &ctx);
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
}

SemanticContext Expr_check_break_or_continue(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr->kind == EXPR_BREAK || expr->kind == EXPR_CONTINUE);

  if (!out->data.inside_loop)
    ctx_err(ERR_SEM_LOOP_KEYWORD_OUTSIDE_LOOP, .token = expr->token);

  ctx_success();
}

SemanticContext Expr_check_fun_def(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  SemanticContext ctx_in = Expr_check(expr->def.fun.body, errs, sco, out);
  ctx_check(ctx_in);

  Symbol new_symbol = {0};
  new_symbol.kind = SYM_FUN;
  new_symbol.fun.body = expr->def.fun.body;
  Scope_insert(sco, new_symbol);
  ctx_success();
}

SemanticContext Expr_check_def(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  Symbol *sym = Scope_search_until_global(sco, token_sv(expr->def.ident));
  if (sym != NULL) {
    ctx_err(ERR_SEM_REDEFINITION, .token = expr->def.ident);
  }
  switch (expr->def.kind) {
  case DEF_FUN:
    return Expr_check_fun_def(expr, errs, sco, out);
  }
  ctx_success();
}

SemanticContext Expr_check(
    Expr *expr, SemanticError_A *errs, Scope *sco, SemanticContext *out
) {
  assert(expr != NULL);

  SemanticContext ctx = {0};
  ctx_propagate_down(out, &ctx);

  switch (expr->kind) {
  case EXPR_LITERAL:
    return Expr_check_literal(expr, errs, sco, &ctx);
  case EXPR_UNARY:
    return Expr_check_unary(expr, errs, sco, &ctx);
  case EXPR_BREAK:
  case EXPR_CONTINUE:
    return Expr_check_break_or_continue(expr, errs, sco, &ctx);
  case EXPR_BINARY:
    return Expr_check_binary(expr, errs, sco, &ctx);
  case EXPR_GROUP:
    return Expr_check_group(expr, errs, sco, &ctx);
  case EXPR_DECL:
    return Expr_check_decl(expr, errs, sco, &ctx);
  case EXPR_BLOCK:
    return Expr_check_block(expr, errs, sco, &ctx);
  case EXPR_IDENT:
    return Expr_check_ident(expr, errs, sco, &ctx);
  case EXPR_PARENT:
    return Expr_check_parent(expr, errs, sco, &ctx);
  case EXPR_IF:
    return Expr_check_if(expr, errs, sco, &ctx);
  case EXPR_WHILE:
    return Expr_check_while(expr, errs, sco, &ctx);
  case EXPR_DEF:
    return Expr_check_def(expr, errs, sco, &ctx);
  case EXPR_ERROR:
    UNREACHABLE("error leaf at semantics?\n");
  }
}

void Semantics_check(
    ExprNode_A *code, Scope *sco, SemanticError_A *errs, size_t max_errs
) {
  da_iter(expr, code) {
    (void) Expr_check(*expr, errs, sco, NULL);
    if (errs->len > max_errs)
      return;
  }
}

void Semantics_print_errs(SemanticError_A *errs, char *file_path, size_t source_len) {
#define SPEC "%s: [%zu](%zu) error:\n"
#define DAT file_path, e->token->start, e->token->line
#define TOK size_t_int(e->token->len), e->token->lexeme
#define P(...) fprintf(stderr, __VA_ARGS__)
#define TYPE0\
  size_t_int(e->type[0]->pattern.name.len), e->type[0]->pattern.name.ptr
#define TYPE1\
  size_t_int(e->type[1]->pattern.name.len), e->type[1]->pattern.name.ptr
#define TYPE_PAT\
  size_t_int(e->type_pattern.ptr_count + e->type_pattern.name.len),\
  _type_pattern_to_str(e->type_pattern)

  SemanticError *e;

  da_for(e, errs) {
    if (e->token == NULL)
      e->token = e->expr->token;

    switch (e->code) {
    case ERR_SEM_UNDEFINED_SYMBOL:
      P(SPEC"undefined symbol '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ALREADY_DECLARED_SYMBOL:
      P(SPEC"already declared symbol '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ASSIGN_TO_FIX:
      P(SPEC"assigned to fixed variable '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_ASSIGN_TO_RVALUE:
      P(SPEC"assigned to R-value '%.*s'\n", DAT, TOK);
      break;
    case ERR_SEM_DECL_AFTER_IF_ELSE:
      P(SPEC"declaration after if-else\n", DAT);
      break;
    case ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE:
      P(SPEC"declaration after while-then-else\n", DAT);
      break;
    case ERR_SEM_INCOMPATIBLE_TYPES:
      P(SPEC"incompatible types: '@%.*s' and '@%.*s'\n", DAT, TYPE0, TYPE1);
      break;
    case ERR_SEM_LOOP_KEYWORD_OUTSIDE_LOOP:
      P(SPEC"used '%.*s' keyword outside loop\n", DAT, TOK);
      break;
    case ERR_SEM_INCOMPATIBLE_OPERATOR:
      P(SPEC"type '%.*s' doesn't work with '%.*s' operator\n", DAT, TYPE0, TOK);
      break;
    case ERR_SEM_UNDEFINED_TYPE:
      P(SPEC"undefined type '@%.*s'\n", DAT, TYPE_PAT);
      break;
    case ERR_SEM_DEREF_NON_POINTER:
      P(SPEC"attempt to dereference type '@%.*s' which is not a pointer\n",
        DAT, TYPE0);
      break;
    case ERR_SEM_REDEFINITION:
      P(SPEC"redefinition of symbol '%.*s'\n", DAT, TOK);
      break;
    }

    char *s = e->token->lexeme;
    // find line beginning
    if (e->token->line > 1) {
      while (*s != '\n') s--;
      s++;
    } else {
      s -= e->token->start;
    }

    // get line length
    int i = -1;
    while (s[++i] != '\n' && i < (source_len - e->token->start));

    fprintf(stderr,
      "%.*s\n",
      i, s);

    for (char *sp = s; sp != e->token->lexeme; sp++) {
      fputc(' ', stderr);
    }
    for (size_t n = 0; n < e->token->len; n++) {
      fputc('^', stderr);
    }
    fputc('\n', stderr);
  }
}
