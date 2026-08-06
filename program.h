#ifndef RAIZ_PROGRAM_H
#define RAIZ_PROGRAM_H

typedef struct {
  enum {
    ERR_SEM_UNDEFINED_SYMBOL, // token
    ERR_SEM_ALREADY_DECLARED_SYMBOL, // token, scope
    ERR_SEM_ASSIGN_TO_VAL, // token
    ERR_SEM_ASSIGN_TO_RVALUE, // token, expr (assignment)
    ERR_SEM_DECL_AFTER_IF_ELSE, // token, expr (declaration)
    ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE, // token, expr (declaration)
  } code;
  // context:
  Token *token;
  Scope *scope;
  Expr *expr;
} SemanticError;
da_make(SemanticError_A, SemanticError*);

typedef struct {
  Token_A *toks;
  Parser *par;
  ExprNode_A code;
  Scope *sco;
} Program;

Program Program_setup(Scope *sco, Parser *par);
void Program_debug(Program *pro, size_t indent);
void Program_free(Program *pro);
int Program_build(Program *pro);
Value Program_run(Program *pro);
void Program_check(Program *pro, SemanticError_A *errs, size_t max_errs);

#endif // RAIZ_PROGRAM_H
