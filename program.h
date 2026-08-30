#ifndef RAIZ_PROGRAM_H
#define RAIZ_PROGRAM_H

typedef struct {
  enum {
    ERR_SEM_UNDEFINED_SYMBOL,
    ERR_SEM_ALREADY_DECLARED_SYMBOL,
    ERR_SEM_ASSIGN_TO_VAL,
    ERR_SEM_ASSIGN_TO_RVALUE,
    ERR_SEM_DECL_AFTER_IF_ELSE,
    ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE,
    ERR_SEM_INCOMPATIBLE_TYPES,
    ERR_SEM_LOOP_KEYWORD_OUTSIDE_LOOP,
    ERR_SEM_INCOMPATIBLE_OPERATOR,
  } code;
  // context:
  Token *token;
  Expr *expr;
  const Type *type[2];
  size_t count;
} SemanticError;
da_make(SemanticError_A, SemanticError*);

typedef struct {
  bool ok;
  const Type *type;
  bool is_lvalue;
  bool is_constant;
  bool is_variable;
  struct {
    bool inside_loop;
  } data;
} SemanticContext;

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
