#ifndef RAIZ_PARSER_C
#define RAIZ_PARSER_C

int parse_literal(Token *tok, Expr *res, Parser *par);
int parse_unary(Token *tok, Expr *res, Parser *par);
int parse_group(Token *tok, Expr *res, Parser *par);
int parse_ident(Token *tok, Expr *res, Parser *par);
int parse_block(Token *tok, Expr *res, Parser *par);
int parse_parent_access(Token *tok, Expr *res, Parser *par);
int parse_if(Token *tok, Expr *res, Parser *par);
int parse_definition(Token *tok, Expr *res, Parser *par);
int parse_function_definition(Token *tok, Expr *res, Parser *par);
int parse_declaration(Token *tok, Expr *res, Parser *par);
int parse_while(Token *tok, Expr *res, Parser *par);
int parse_type_pattern(TypePattern *res, Parser *par);
int parse_nud(Expr *res, Parser *par);
int parse_expr(Expr *res, Parser *par, uint8_t min_bp);
int parse_line(Expr *res, Parser *par);

#undef current
#undef peek
#undef advance
#undef error
#undef expect
#undef expect_flag
#undef expect_block


#define peek() Parser_peek(par)
#define current() Parser_current(par)
#define advance() Parser_advance(par)

#define error(__code, __token, ...)\
do {\
  Expr *err_expr = Expr_();\
  err_expr->kind = EXPR_ERROR;\
  err_expr->error.tok = __token;\
  err_expr->error.code = __code;\
  snprintf(err_expr->error.msg, sizeof(err_expr->error.msg), __VA_ARGS__);\
  da_add(&par->errs, err_expr);\
  debug("parse error (with token '%s' at (%zu:%zu))\n",\
    token_string(__token), (__token)->line, (__token)->column);\
  return __code;\
} while (0)

#define expect(__token, __kind, ...)\
do {\
  if ((__token)->kind != (__kind)) {\
    error(PARSER_EXPECTATION_FAILED, __token,\
      "expected '%s', found '%s'\n",\
      token_name(__kind), token_string(__token));\
  }\
} while (0)

#define expect_block(__token, __after_what)\
expect(__token, TOKEN_L_CURLY, "after `"__after_what"`")

#define expect_flag(__token, __flag, ...)\
do {\
  if (!((__token)->flags & (__flag))) {\
    error(PARSER_EXPECTATION_FAILED, __token, __VA_ARGS__);\
  }\
} while (0)

#define consume(__token, __kind)\
do {\
  expect(__token, __kind);\
  advance();\
} while (0)

int parse_literal(Token *tok, Expr *res, Parser *par) {
  res->kind = EXPR_LITERAL;
  res->literal = tok;
  return 0;
}

int parse_unary(Token *tok, Expr *res, Parser *par) {
  uint8_t bp = get_binding_power(tok->kind);
  if (tok->flags & TOKEN_FLAG_RIGHT_ASSOCIATIVE)
    bp -= 1;

  advance();

  Expr *in = Expr_();
  int err = parse_expr(in, par, bp);
  if (err)
    return err;

  res->kind = EXPR_UNARY;
  res->unary.op = tok;
  res->unary.in = in;
  return 0;
}

int parse_group(Token *tok, Expr *res, Parser *par) {
  advance();

  Expr *in = Expr_();
  int err = parse_expr(in, par, 0);
  if (err)
    return err;

  Token *peeked = peek();
  expect(peeked, TOKEN_R_PAREN, "closing ')'");
  advance();

  res->kind = EXPR_GROUP;
  res->group.in = in;
  return 0;
}

int parse_function_call(Token *tok, Expr *res, Parser *par) {
  consume(tok, TOKEN_IDENT);
  consume(tok + 1, TOKEN_L_PAREN);

  res->funcall.ident = tok;
  tok = current();

#if 0
  tok = current();
  ExprNode_A args = {0};
  while (tok->flags & TOKEN_FLAG_STARTER) {
    Expr *arg = Expr_();
    int err = parse_expr(arg, par, 0);
    if (err)
      return err;
    da_add(&args, arg);
    tok = current();
    Token *peeked = peek();
    expect_flag(peeked, TOKEN_FLAG_SEPARATOR, "seperator");
    advance();
  }
  res->funcall.args = args;
#endif

  consume(tok, TOKEN_R_PAREN);
  res->kind = EXPR_FUNCALL;
  return 0;
}

int parse_ident(Token *tok, Expr *res, Parser *par) {
  Token *peeked = peek();
  if (peeked->kind == TOKEN_L_PAREN)
    return parse_function_call(tok, res, par);
  res->kind = EXPR_IDENT;
  res->ident = tok;
  return 0;
}

int parse_block(Token *tok, Expr *res, Parser *par) {
  // consume(tok, TOKEN_L_CURLY);
  advance();

  do {
    Expr *line = Expr_();

    int err = parse_line(line, par);
    if (err)
      return err;

    // local pointer!
    tok = peek();

    da_add(&res->block, line);
  } while (par->cur < par->toks->len && tok->kind != TOKEN_R_CURLY);

  expect(tok, TOKEN_R_CURLY, "closing '}'");
  advance();

  res->kind = EXPR_BLOCK;
  return 0;
}

int parse_parent_access(Token *tok, Expr *res, Parser *par) {
  uint32_t level = 0;
  do {
    level++;
    tok = advance();
  } while (current()->kind == TOKEN_HAT);

  expect(tok, TOKEN_IDENT, "identifier");

  res->kind = EXPR_PARENT;
  res->parent.level = level;
  res->parent.ident = tok;
  return 0;
}

int parse_if(Token *tok, Expr *res, Parser *par) {
  consume(tok, TOKEN_IF);

  Expr *cond = Expr_();
  int err = parse_expr(cond, par, 0);
  if (err)
    return err;

  Token *peeked = peek();
  expect_block(peeked, "if");
  advance();

  Expr *then_branch = Expr_();
  err = parse_block(peeked, then_branch, par);
  if (err)
    return err;

  Expr *else_branch = NULL;
  peeked = peek();

  if (peeked->kind == TOKEN_ELSE) {
    advance();

    peeked = peek();
    expect_block(peeked, "else");
    advance();

    else_branch = Expr_();
    err = parse_block(peeked, else_branch, par);
    if (err)
      return err;
  }

  res->kind = EXPR_IF;
  res->if_node.cond = cond;
  res->if_node.then_branch = then_branch;
  res->if_node.else_branch = else_branch;
  return 0;
}

int parse_function_definition(Token *tok, Expr *res, Parser *par) {
  expect(tok, TOKEN_IDENT, "identifier");

  Token *peeked = peek();
  advance();

  expect_block(peeked, "fun");
  advance();

  Expr *body = Expr_();
  int err = parse_block(peeked, body, par);
  if (err)
    return err;

  res->def.kind = DEF_FUN;
  res->def.fun.body = body;
  return 0;
}

int parse_definition(Token *tok, Expr *res, Parser *par) {
  expect(tok, TOKEN_IDENT, "identifier");
  res->kind = EXPR_DEF;
  res->ident = tok;

  Token *peeked = peek();
  expect(peeked, TOKEN_COLLON_X2, "'::'");
  // no consume since they were checked
  advance(); // identifier
  advance(); // '::'

  Token *variant = current();
  expect_flag(variant, TOKEN_FLAG_DECLARATOR, "declarator");
  switch (variant->kind) {
  case TOKEN_FUN:
    return parse_function_definition(tok, res, par);
  default:
    UNREACHABLE("should '%s' be a declarator?\n", token_string(variant));
    break;
  }
  return 0;
}

int parse_declaration(Token *tok, Expr *res, Parser *par) {
  expect(tok, TOKEN_IDENT, "identifier");

  Token *peeked = peek();
  expect(peeked, TOKEN_COLLON, "declaration");

  consume(tok, TOKEN_IDENT);
  consume(peeked, TOKEN_COLLON);

  TypePattern type = {0};
  int err = parse_type_pattern(&type, par);
  if (err)
    return err;

  peeked = peek();
  if (peeked->kind == TOKEN_EQUAL) {
    advance();
    consume(peeked, TOKEN_EQUAL);

    Expr *value = Expr_();
    err = parse_expr(value, par, 0);
    if (err)
      return err;

    res->decl.value = value;
  }

  res->kind = EXPR_DECL;
  res->decl.ident = tok;
  res->decl.type = type;
  return 0;
}

int parse_while(Token *tok, Expr *res, Parser *par) {
  advance();

  Expr *cond = Expr_();
  int err = parse_expr(cond, par, 0);
  if (err)
    return err;

  Token *peeked = peek();
  expect_block(peeked, "while");
  advance();

  Expr *body = Expr_();
  err = parse_block(peeked, body, par);
  if (err)
    return err;

  Expr *then_branch = NULL;
  if (peek()->kind == TOKEN_THEN) {
    advance();

    peeked = peek();
    expect_block(peeked, "then");
    advance();

    then_branch = Expr_();
    err = parse_block(peeked, then_branch, par);
    if (err)
      return err;
  }

  Expr *else_branch = NULL;
  if (peek()->kind == TOKEN_ELSE) {
    advance();

    peeked = peek();
    expect_block(peeked, "else");
    advance();

    else_branch = Expr_();
    err = parse_block(peeked, else_branch, par);
    if (err)
      return err;
  }

  res->kind = EXPR_WHILE;
  res->while_node.cond = cond;
  res->while_node.body = body;
  res->while_node.then_branch = then_branch;
  res->while_node.else_branch = else_branch;
  return 0;
}

int parse_type_pattern(TypePattern *res, Parser *par) {
  Token *tok = current();
  expect(tok, TOKEN_AT, "'@'");

  tok = advance();
  expect(tok, TOKEN_IDENT, "type identifier");
  res->name = token_sv(tok);

  Token *peeked = peek();
  while (peeked->kind == TOKEN_STAR) {
    res->ptr_count++;
    advance();
    peeked = peek();
  }

  return 0;
}

int parse_nud(Expr *res, Parser *par) {
  Token *tok = current();
  res->token = tok;

  if (tok->kind == TOKEN_INVALID)
    error(PARSER_INVALID_TOKEN, tok, "invalid token");

  else if (tok->flags & TOKEN_FLAG_CONSTANT) {
    return parse_literal(tok, res, par);
  } else if (tok->flags & TOKEN_FLAG_UNARY) {
    return parse_unary(tok, res, par);
  } else if (tok->kind == TOKEN_L_PAREN) {
    return parse_group(tok, res, par);
  } else if (tok->kind == TOKEN_IDENT) {
    return parse_ident(tok, res, par);
  } else if (tok->kind == TOKEN_L_CURLY) {
    return parse_block(tok, res, par);
  } else if (tok->kind == TOKEN_HAT) {
    return parse_parent_access(tok, res, par);
  } else if (tok->kind == TOKEN_IF) {
    return parse_if(tok, res, par);
  } else {
    error(PARSER_UNEXPECTED_TOKEN, tok,
      "unexpected token: '%s'\n", token_string(tok));
  }
  return 0;
}

int parse_expr(Expr *ls, Parser *par, uint8_t min_bp) {
  Token *first = current();
  int err = parse_nud(ls, par);
  if (err)
    return err;

  Expr *res = NULL;
  Token *op;
  while (!((op = peek())->flags & TOKEN_FLAG_BREAKING)) {
    expect_flag(op, TOKEN_FLAG_OPERATOR, "expected operator");

    uint8_t bp = get_binding_power(op->kind);
    if (op->flags & TOKEN_FLAG_RIGHT_ASSOCIATIVE)
      bp -= 1;

    if (bp < min_bp)
      break;

    // to figure out: why do we need two advances?
    advance();
    advance();

    Expr *rs = Expr_();
    err = parse_expr(rs, par, bp);
    if (err)
      return err;

    res = Expr_();
    res->kind = EXPR_BINARY;
    res->binary.ls = Expr_copy(ls);
    res->binary.op = op;
    res->binary.rs = rs;
    memcpy(ls, res, sizeof(*ls));
  }

  ls->token = first;
  if (res != NULL)
    res->token = first;
  return 0;
}

// a line is like a statement, expression and then a TOKEN_NEWLINE
int parse_line(Expr *res, Parser *par) {
  Token *tok = current();
  while (tok->flags & TOKEN_FLAG_FINISHER && tok->kind != TOKEN_EOF)
    tok = advance();

  if (tok->kind == TOKEN_EOF)
    return -1;

  int err = 0;
  Token *first = tok;
  if (!(tok->flags & TOKEN_FLAG_STARTER)) {
    err = parse_expr(res, par, 0);
    if (err)
      return err;

    goto finish_line;
  }
  switch (tok->kind) {
  case TOKEN_IDENT: {
    Token *peeked = peek();
    if (peeked->kind == TOKEN_COLLON_X2) {
      return parse_definition(tok, res, par);
    } else if (peeked->kind == TOKEN_COLLON) {
      return parse_declaration(tok, res, par);
    } else {
      err = parse_expr(res, par, 0);
      if (err)
        return -1;
      goto finish_line;
    }
  } break;
  case TOKEN_WHILE:
    return parse_while(tok, res, par);
  case TOKEN_BREAK:
    res->kind = EXPR_BREAK;
    break;
  case TOKEN_CONTINUE:
    res->kind = EXPR_CONTINUE;
    break;
  default: UNREACHABLE("token %s\n", token_name(tok->kind));
  }

finish_line:
  expect_flag(peek(), TOKEN_FLAG_FINISHER, "new line or ';'");
  advance();
  res->token = first;
  return 0;
}

int parse_program(Parser *par) {
  Token *tok = NULL;
  while ((tok = current())->kind != TOKEN_EOF) {
    Expr *node = Expr_();

    int res = parse_line(node, par);
    if (res > 0) {
      da_iter(expr, &par->errs) {
        Expr_Error *err = &(*expr)->error;
        fprintf(stderr, "parser error [%zu](%zu:%zu): ",
          err->tok->start, err->tok->line, err->tok->column);
        fprintf(stderr, "%s\n", err->msg);
      }
      // Token *t;
      // da_for(t, par->toks) {
      //   fprintf(stderr, "token #%zu: %s,", i_t+1, token_string(t));
      //   if (i_t == par->cur)
      //     fprintf(stderr, " // current\n");
      //   else
      //     fprintf(stderr, "\n");
      // }
      return res;
    } else if (res < 0)
      continue;

    da_add(&par->ast, node);
    advance();
  }
  return 0;
}

Parser Parser_setup(Token_A *toks) {
  return (Parser) {.toks = toks, .cur = 0};
}

uint8_t get_binding_power(enum TokenKind kind) {
  switch(kind) {
  case TOKEN_EQUAL:       return 10;  // =
  case TOKEN_BANG:        return 20;  // !
  case TOKEN_PIPE_X2:     return 30;  // ||
  case TOKEN_AMPER_X2:    return 40;  // &&
  case TOKEN_TILDE:       return 50;  // ~
  case TOKEN_PIPE:        return 60;  // |
  case TOKEN_HAT:         return 70;  // ^
  case TOKEN_AMPER:       return 80;  // &
  case TOKEN_EQUAL_X2:                // ==
  case TOKEN_BANG_EQUAL:  return 90;  // !=
  case TOKEN_LESS:                    // <
  case TOKEN_LESS_EQUAL:              // <=
  case TOKEN_GREAT:                   // >
  case TOKEN_GREAT_EQUAL: return 100; // >=
  case TOKEN_LESS_X2:                 // <<
  case TOKEN_GREAT_X2:    return 110; // >>
  case TOKEN_PLUS:                    // +
  case TOKEN_MINUS:       return 120; // -
  case TOKEN_STAR:                    // *
  case TOKEN_SLASH:       return 130; // /
  default: PANIC("token is not an operator or is unhandled (id %d)\n", kind);
  }
}

Expr *Expr_(void) {
  Expr *e = malloc(sizeof(*e));
  if (e)
    memset(e, 0, sizeof(*e));
  return e;
}

Expr *Expr_copy(Expr *src) {
  Expr *e = malloc(sizeof(*e));
  if (e)
    memcpy(e, src, sizeof(*e));
  return e;
}

void Expr_free(Expr *node) {
  if (node == NULL)
    return;

  switch (node->kind) {
  case EXPR_LITERAL:
  case EXPR_IDENT:
  case EXPR_PARENT:
  case EXPR_BREAK:
  case EXPR_CONTINUE:
  case EXPR_FUNCALL:
    break;
  case EXPR_BINARY:
    Expr_free(node->binary.ls);
    Expr_free(node->binary.rs);
    break;
  case EXPR_UNARY:
    Expr_free(node->unary.in);
    break;
  case EXPR_GROUP:
    Expr_free(node->group.in);
    break;
  case EXPR_DECL:
    if (node->decl.value != NULL)
      Expr_free(node->decl.value);
    break;
  case EXPR_DEF:
    switch (node->def.kind) {
    case DEF_FUN:
      Expr_free(node->def.fun.body);
      break;
    }
    break;
  case EXPR_BLOCK:
    da_iter(line, &node->block) {
      Expr_free(*line);
    }
    break;
  case EXPR_IF:
    Expr_free(node->if_node.cond);
    Expr_free(node->if_node.then_branch);
    Expr_free(node->if_node.else_branch);
    break;
  case EXPR_WHILE:
    Expr_free(node->while_node.cond);
    Expr_free(node->while_node.body);
    Expr_free(node->while_node.then_branch);
    Expr_free(node->while_node.else_branch);
    break;
  case EXPR_ERROR:
    // nothing to free here...
    break;
  }
  free(node);
}

Token *Parser_current(Parser *par) { return &par->toks->dat[par->cur]; }
Token *Parser_peek(Parser *par) { return &par->toks->dat[par->cur+1]; }
// bruh, `next()` is never used!
// Token *Parser_next(Parser *par) { return &par->toks->dat[par->cur++]; }
Token *Parser_advance(Parser *par) { return &par->toks->dat[++par->cur]; }

#undef current
#undef peek
#undef advance
#undef expect
#undef expect_flag
#undef expect_block

#endif // RAIZ_PARSER_C
