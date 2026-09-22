#ifndef RAIZ_SEMANTICS_H
#define RAIZ_SEMANTICS_H

typedef struct {
  enum {
    ERR_SEM_UNDEFINED_SYMBOL,
    ERR_SEM_UNDEFINED_TYPE,
    ERR_SEM_ALREADY_DECLARED_SYMBOL,
    ERR_SEM_ASSIGN_TO_FIX,
    ERR_SEM_ASSIGN_TO_RVALUE,
    ERR_SEM_DECL_AFTER_IF_ELSE,
    ERR_SEM_DECL_AFTER_WHILE_THEN_ELSE,
    ERR_SEM_INCOMPATIBLE_TYPES,
    ERR_SEM_LOOP_KEYWORD_OUTSIDE_LOOP,
    ERR_SEM_INCOMPATIBLE_OPERATOR,
    ERR_SEM_DEREF_NON_POINTER,
  } code;
  // context:
  Token *token;
  Expr *expr;
  Type *type[2];
  TypePattern type_pattern;
  size_t count;
} SemanticError;
da_make(SemanticError_A, SemanticError*);

typedef struct {
  bool err : 1;
  bool is_lvalue : 1;
  bool is_constant : 1;
  uint8_t deref_count;
  struct {
    bool inside_loop : 1;
  } data;
  Type *type;
} SemanticContext;

#endif /* RAIZ_SEMANTICS_H */
