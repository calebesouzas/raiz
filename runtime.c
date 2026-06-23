#ifndef RAIZ_RUNTIME_C
#define RAIZ_RUNTIME_C

int eval(Expr *e) {
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
      return -eval(e->unary.in);
    }
  case EXPR_BINARY: {
    int ls = eval(e->binary.ls);
    int rs = eval(e->binary.rs);
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
    return eval(e->group.in);
  }
  return 0;
}

#endif // RAIZ_RUNTIME_C
