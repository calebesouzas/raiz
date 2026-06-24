#ifndef RAIZ_PROGRAM_H
#define RAIZ_PROGRAM_H

da_make(ExprNode_A, Expr**);

typedef struct {
  Token_A *toks;
  ExprNode_A code;
  Symbol_A *syms;
} Program;

Program Program_setup(Symbol_A *syms, Parser *par);
void Program_debug(Program *pro, size_t indent);
int Program_build(Program *pro, Parser *par);
int Program_run(Program *pro);

#endif // RAIZ_PROGRAM_H
