#ifndef RAIZ_PROGRAM_H
#define RAIZ_PROGRAM_H

typedef struct {
  Token_A *toks;
  Parser *par;
  ExprNode_A code;
  Scope *sco;
} Program;

Program Program_setup(Scope *sco, Parser *par);
void Program_free(Program *pro);
int Program_build(Program *pro);
Value Program_run(Program *pro);
void Program_check(Program *pro, SemanticError_A *errs, size_t max_errs);

#endif // RAIZ_PROGRAM_H
