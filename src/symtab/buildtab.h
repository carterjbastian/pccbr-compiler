#include "../util/ast.h"
#include "symtab.h"

symboltable_t *build_symboltable(symboltable_t *table, ast_node root, ast_node curr);