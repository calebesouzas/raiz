#pragma once

#include "libc.h"
#include "generics.h"
#include "ast.h"

bool check_ast(Ast *ast);

#define UNDECLARED_REPORT_LIMIT 5
