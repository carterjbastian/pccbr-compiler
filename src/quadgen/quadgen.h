#ifndef QUAD_GEN_
#define QUAD_GEN_

#include <stdio.h>

#include "../util/ast.h"
#include "../util/symtab.h"

typedef enum {
  /* NULL */
  NULL_QOP,
  /* ASSIGN */
  ASSN_QOP,
  /* ARITHMETIC */
  ADD_QOP, SUB_QOP, MULL_QOP, DIV_QOP, MOD_QOP,
  /* MOVEMENT */
  GOTO_QOP, IF_FALSE_QOP, IF_TRUE_QOP,
  /* CONDITIONALS */
  EQ_QOP, LT_QOP, GT_QOP, LTE_QOP, GTE_QOP,
  /* LABELING */
  LABEL_QOP,
  /* HIGH LEVEL FUNCTIONS */
  PRINT_QOP, READ_QOP,
  /* FUNCTION CALL HANDLING */
  PRE_CALL_QOP, POST_RET_QOP, RETURN_QOP, ARG_QOP
} optype;

typedef struct quad_struct *quad_t;
struct quad_struct {
    optype op;
    symnode_t *operand1;
    symnode_t *operand2;
    symnode_t *operand3;

    quad_t next;
};

symnode_t *code_gen(ast_node node, symboltable_t *table);

symnode_t *NewTemp(symboltable_t *table);

void print_quad_list(FILE *fp, quad_t list);
#endif // QUAD_GEN_

