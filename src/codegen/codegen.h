#ifndef CODE_GEN_
#define CODE_GEN_

#include <stdio.h>

#include "../util/ast.h"
#include "../util/symtab.h"
#include "../quadgen/quadgen.h"


typedef struct reg {
    char *name;
    symnode_t *contents;
    int available;
} reg_t;


int generate_assembly(FILE *fp, quad_t ir, symboltable_t *table);

reg_t *get_available_register(FILE *fp, reg_t *registers);

#endif // CODE_GEN_

