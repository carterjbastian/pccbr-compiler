#include "quadgen.h"
#include <stdio.h>

quad_t create_quad(optype op, symnode_t *operand1, symnode_t *operand2, symnode_t *operand3) {
  quad_t new_quad = calloc(1, sizeof(struct quad_struct));
  new_quad->op = op;
  new_quad->operand1 = operand1;
  new_quad->operand2 = operand2;
  new_quad->operand3 = operand3;

  return new_quad;
}

void code_gen(ast_node node, symboltable_t *table) {
  ast_node child = node->left_child, rsib = node->right_sibling;
  int changed_scope = 0;

  if (node->node_type == FUNC_N || node->node_type == COMPOUND_STMT_N) {
    // Change to next child scope
    change_scope(table);
    changed_scope = 1; 
  }

  /* Recurse on Children */
  if (child)
    code_gen(child, table);
  
  if (changed_scope) {
    table->leaf = table->leaf->parent;
  }

  if (rsib)
    code_gen(rsib, table);
  printf("%s\n", NODE_NAME(node->node_type));
}


