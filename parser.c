#ifndef RAIZ_PARSER_C
#define RAIZ_PARSER_C

#undef expect
#define expect(__what, __token, __err_variant)\
  do{\
    fprintf(stderr, "expected "__what", found %s\n", token_label(__token));\
    return __err_variant;\
  } while (0)

int Parser_parse_nud(Expr *res, Parser *par) {
  Token *tok, *peeked, *first;
  Expr *in, *value, *line;
  int err;

  tok = Parser_cur(par);
  first = tok;

  if (tok->kind == TOKEN_INVALID)
    return PARSER_INVALID_TOKEN;

  else if (tok->flags & TOKEN_FLAG_CONSTANT) {
    res->kind = EXPR_LITERAL;
    res->literal = tok;
  } else if (tok->flags & TOKEN_FLAG_UNARY) {
    uint8_t bp = get_binding_power(tok->kind);
    if (tok->flags & TOKEN_FLAG_RIGHT_ASSOCIATIVE)
      bp -= 1;

    Parser_advance(par);

    in = Expr_();
    err = Parser_parse_expr(in, par, bp);
    if (err)
      return err;

    res->kind = EXPR_UNARY;
    res->unary.op = tok;
    res->unary.in = in;
  } else if (tok->kind == TOKEN_L_PAREN) {
    Parser_advance(par);

    in = Expr_();
    err = Parser_parse_expr(in, par, 0);
    if (err)
      return err;

    peeked = Parser_peek(par);
    if (peeked->kind != TOKEN_R_PAREN) {
      expect("closing ')'", peeked, PARSER_NOT_CLOSED_GROUP);
    }
    Parser_advance(par); // consume ')'

    res->kind = EXPR_GROUP;
    res->group.in = in;
  } else if (tok->kind == TOKEN_IDENT) {
    res->kind = EXPR_IDENT;
    res->ident = tok;
    res->token = first;
  } else if (tok->kind == TOKEN_L_CURLY) {
    Parser_advance(par); // `{`
    do {
      line = Expr_();

      err = Parser_parse_line(line, par);
      if (err)
        return err;

      peeked = Parser_peek(par);

      da_add(&res->block, line);
    } while (par->cur < par->toks->len && peeked->kind != TOKEN_R_CURLY);

    if (peeked->kind != TOKEN_R_CURLY) {
      //@todo print start line when we track line numbers
      expect("closing '}'", peeked, PARSER_NOT_CLOSED_BLOCK);
    }
    Parser_advance(par); // before '}'

    res->kind = EXPR_BLOCK;
  } else if (tok->kind == TOKEN_HAT) {
    uint32_t level = 0;
    do {
      level++;
      tok = Parser_advance(par);
    } while (Parser_cur(par)->kind == TOKEN_HAT);
    if (tok->kind != TOKEN_IDENT) {
      expect("identifier", tok, PARSER_EXPECTED_IDENTIFIER);
    }

    res->kind = EXPR_PARENT;
    res->parent.level = level;
    res->parent.ident = tok;
  } else if (tok->kind == TOKEN_IF) {
    Parser_advance(par);

    Expr *cond = Expr_();
    err = Parser_parse_expr(cond, par, 0);
    if (err)
      return err;

    peeked = Parser_peek(par);
    if (peeked->flags & TOKEN_FLAG_STARTER) {
      expect("block after `if`", peeked, PARSER_EXPECTED_EXPRESSION);
    }
    Parser_advance(par);

    Expr *then_branch = Expr_();
    err = Parser_parse_expr(then_branch, par, 0);
    if (err)
      return err;

    Expr *else_branch = NULL;
    peeked = Parser_peek(par);

    if (peeked->kind == TOKEN_ELSE) {
      Parser_advance(par);

      peeked = Parser_peek(par);
      if (peeked->flags & TOKEN_FLAG_STARTER) {
        expect("block after `else`", peeked, PARSER_EXPECTED_EXPRESSION);
      }

      Parser_advance(par);
      else_branch = Expr_();
      err = Parser_parse_expr(else_branch, par, 0);
      if (err)
        return err;
    }

    res->kind = EXPR_IF;
    res->if_node.cond = cond;
    res->if_node.then_branch = then_branch;
    res->if_node.else_branch = else_branch;
  } else if (tok->kind == TOKEN_READ) {
    res->kind = EXPR_READ;
  } else if (tok->kind == TOKEN_PRINT) {
    Parser_advance(par);

    value = Expr_();
    err = Parser_parse_expr(value, par, 0);
    if (err)
      return err;

    res->kind = EXPR_PRINT;
    res->print.value = value;
  } else {
    fprintf(stderr, "unexpected token: %s\n", token_label(tok));
    return PARSER_UNEXPECTED_TOKEN;
  }
  res->token = first;
  return 0;
}

int Parser_parse_expr(Expr *ls, Parser *par, uint8_t min_bp) {
  Expr *rs;
  Expr *res = NULL;
  Token *op, *first;
  uint8_t bp;
  int err;

  first = Parser_cur(par);
  err = Parser_parse_nud(ls, par);
  if (err)
    return err;

  while (!((op = Parser_peek(par))->flags & TOKEN_FLAG_BREAKING)) {
    if (!(op->flags & TOKEN_FLAG_OPERATOR)) {
      expect("operator", op, PARSER_EXPECTED_OPERATOR);
    }

    bp = get_binding_power(op->kind);
    if (op->flags & TOKEN_FLAG_RIGHT_ASSOCIATIVE)
      bp -= 1;

    if (bp < min_bp)
      break;

    // to figure out: why do we need two advances?
    Parser_advance(par);
    Parser_advance(par);

    rs = Expr_();
    err = Parser_parse_expr(rs, par, bp);
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

int Parser_parse_line(Expr *res, Parser *par) {
  int err;
  Token *tok, *peeked, *first;
  Expr *value;

  tok = Parser_cur(par);
  first = tok;
  while (tok->flags & TOKEN_FLAG_FINISHER)
    tok = Parser_advance(par);

  if (!(tok->flags & TOKEN_FLAG_STARTER)) {
parse_expr:
    err = Parser_parse_expr(res, par, 0);
    if (err)
      return err;

    goto finish_line;
  }
  switch (tok->kind) {
  case TOKEN_IDENT:
    peeked = Parser_peek(par);
    if (peeked->kind != TOKEN_COLLON) {
      expect("collon", peeked, PARSER_EXPECTED_COLLON);
    }
    Parser_advance(par); // consume identifier

    Token *kind = Parser_peek(par); // declaration kind
    if (!(kind->flags & TOKEN_FLAG_DECLARATOR)) {
      expect("declarator", kind, PARSER_EXPECTED_DECLARATOR);
    }

    Parser_advance(par); // consume collon
    Parser_advance(par); // consume declarator

    TypePattern type = {0};
    err = Parser_parse_type(&type, par);
    if (err)
      return err;

    peeked = Parser_peek(par);

    if (peeked->kind == TOKEN_EQUAL) {
      Parser_advance(par); // type ending
      Parser_advance(par); // '='

      value = Expr_();
      err = Parser_parse_expr(value, par, 0);
      if (err)
        return err;

      res->decl.value = value;
    } else if (kind->kind == TOKEN_VAL) { // bruh
      expect("assignment", peeked, PARSER_EXPECTED_ASSIGNMENT);
    }

    res->kind = EXPR_DECL;
    res->decl.kind = kind;
    res->decl.ident = tok;
    res->decl.type = type;

    break; // case IDENT (for declaration)
  case TOKEN_WHILE:
    Parser_advance(par);

    Expr *cond = Expr_();
    err = Parser_parse_expr(cond, par, 0);
    if (err)
      return err;

    peeked = Parser_peek(par);
    if (peeked->flags & TOKEN_FLAG_STARTER) {
      expect("block after `while`", peeked, PARSER_EXPECTED_EXPRESSION);
    }
    Parser_advance(par);

    Expr *body = Expr_();
    err = Parser_parse_expr(body, par, 0);
    if (err)
      return err;

    Expr *then_branch = NULL;
    peeked = Parser_peek(par);

    if (peeked->kind == TOKEN_THEN) {
      Parser_advance(par);

      peeked = Parser_peek(par);
      if (peeked->flags & TOKEN_FLAG_STARTER) {
        expect("block after `then`", peeked, PARSER_EXPECTED_EXPRESSION);
      }

      Parser_advance(par);
      then_branch = Expr_();
      err = Parser_parse_expr(then_branch, par, 0);
      if (err)
        return err;
    }

    Expr *else_branch = NULL;
    peeked = Parser_peek(par);

    if (peeked->kind == TOKEN_ELSE) {
      Parser_advance(par);

      peeked = Parser_peek(par);
      if (peeked->flags & TOKEN_FLAG_STARTER) {
        expect("block after `else`", peeked, PARSER_EXPECTED_EXPRESSION);
      }

      Parser_advance(par);
      else_branch = Expr_();
      err = Parser_parse_expr(else_branch, par, 0);
      if (err)
        return err;
    }

    res->kind = EXPR_WHILE;
    res->while_node.cond = cond;
    res->while_node.body = body;
    res->while_node.then_branch = then_branch;
    res->while_node.else_branch = else_branch;
    break;
  case TOKEN_BREAK:
    res->kind = EXPR_BREAK;
    break;
  case TOKEN_CONTINUE:
    res->kind = EXPR_CONTINUE;
    break;
  default: UNREACHABLE("token %s\n", token_label(tok));
  }

finish_line:
  if (!((peeked = Parser_peek(par))->flags & TOKEN_FLAG_FINISHER)) {
    expect("new line or ';'", peeked, PARSER_EXPECTED_FINISH);
  }

  Parser_advance(par);
  res->token = first;
  return 0;
}

int Parser_parse_type(TypePattern *res, Parser *par) {
  Token *tok = Parser_cur(par);
  if (tok->kind != TOKEN_AT) {
    expect("'@'", tok, PARSER_EXPECTED_TYPE);
  }
  tok = Parser_advance(par);

  while (tok->kind == TOKEN_STAR) {
    res->ptr_count++;
    tok = Parser_advance(par);
  }

  if (tok->kind != TOKEN_IDENT) {
    expect("type name", tok, PARSER_EXPECTED_TYPE);
  }
  res->name = token_sv(tok);

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
  Expr **expr;

  if (node == NULL)
    return;

  switch (node->kind) {
  case EXPR_LITERAL:
  case EXPR_IDENT:
  case EXPR_PARENT:
  case EXPR_BREAK:
  case EXPR_CONTINUE:
  case EXPR_READ:
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
  case EXPR_BLOCK:
    da_for(expr, &node->block) {
      Expr_free(*expr);
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
  case EXPR_PRINT:
    Expr_free(node->print.value);
    break;
  }
  free(node);
}

Token *Parser_cur(Parser *par) { return &par->toks->dat[par->cur]; }
Token *Parser_peek(Parser *par) { return &par->toks->dat[par->cur+1]; }
Token *Parser_next(Parser *par) { return &par->toks->dat[par->cur++]; }
Token *Parser_advance(Parser *par) { return &par->toks->dat[++par->cur]; }

#endif // RAIZ_PARSER_C

