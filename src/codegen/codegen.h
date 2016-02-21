#ifndef CODE_GEN_
#define CODE_GEN_

#include <stdio.h>

#include "../util/ast.h"
#include "../util/symtab.h"
#include "../quadgen/quadgen.h"

int generate_assembly(FILE *fp, quad_t ir, symboltable_t *table);

#endif // CODE_GEN_

