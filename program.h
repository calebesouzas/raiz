#ifndef RAIZ_PROGRAM_H
#define RAIZ_PROGRAM_H

typedef struct {
  Token_A *toks;
  Parser *par;
  ExprNode_A code;
  Scope *sco;
} Program;

Program Program_setup(Scope *sco, Parser *par);
void Program_debug(Program *pro, size_t indent);
int Program_build(Program *pro);
int Program_run(Program *pro);

#endif // RAIZ_PROGRAM_H
