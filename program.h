#ifndef RAIZ_PROGRAM_H
#define RAIZ_PROGRAM_H

typedef struct {
  enum {
    ERR_SEM_UNDEFINED_SYMBOL,
    ERR_SEM_ALREADY_DECLARED_SYMBOL,
    ERR_SEM_ASSIGN_TO_VAL,
    ERR_SEM_ASSIGN_TO_RVALUE,
  } code;
  Token *token;
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
int Program_run(Program *pro);
void Program_check(Program *pro, SemanticError_A *errs, size_t max_errs);

#endif // RAIZ_PROGRAM_H
