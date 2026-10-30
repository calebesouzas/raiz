#pragma once

#include "libc.h"
#include "ast.h"
#include "scope.h"

bool transpile_ast(Ast *ast, const char *output_path);
