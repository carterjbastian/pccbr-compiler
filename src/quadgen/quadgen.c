#include "quadgen.h"
#include <stdio.h>

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


