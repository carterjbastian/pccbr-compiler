#ifndef CODE_GEN_
#define CODE_GEN_

#include <stdio.h>

#include "../util/ast.h"
#include "../util/symtab.h"

int code_gen(FILE *fp, quad_t ir, symboltable_t *table);

#endif // CODE_GEN_

