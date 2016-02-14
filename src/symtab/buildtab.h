#ifndef BUILD_TAB_
#define BUILD_TAB_

#include "../util/ast.h"
#include "../util/symtab.h"
#include <string.h>

symboltable_t *build_symboltable(symboltable_t *table, ast_node root, ast_node curr);

#endif // BUILD_TAB_
