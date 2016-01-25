/* ast.c
 *
 *
 * Written by THC for CS 57; massaged by SWS
 *
 * Relies on an enum and a struct for
 * various node types that appear in ast.h.  You should modify the
 * enum and struct as appropriate.
 *
 * This file contains functions to create a node and to print out an
 * abstract syntax tree, for debugging.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ast.h"

/* Create a node with a given token type and return a pointer to the
   node. */
ast_node create_ast_node(ast_node_type node_type) {
  ast_node new_node = calloc(1,sizeof(struct ast_node_struct));  // for zeros
  new_node->node_type = node_type;
  return new_node;
}

/* Print the contents of a subtree of an abstract syntax tree, given
   the root of the subtree and the depth of the subtree root. */
void print_ast(ast_node root, int depth) {
  /* Print two spaces for every level of depth. */
  int i;

  assert(root);

  for (i = 0; i < depth; i++)
    printf("  ");

  /* Print the node type. */
  printf("%s ", NODE_NAME(root->node_type));

  /* Print attributes specific to node types. */
  switch (root->node_type) {
  case ID_N:			/* print the id */
    assert(root->value_string);
    printf("%s", root->value_string);
    break;

  case INT_LITERAL_N:		/* print the int literal */
    printf("%d", root->value_int);
    break;

  case NULL_N:
    printf("%s", root->value_string);
    break;

  case ERROR_N:
    printf("\t%s", root->value_string);
    break;

  default:
    break;
  }

  printf("\n");

  /* Recurse on each child of the subtree root, with a depth one
     greater than the root's depth. */
  ast_node child;
  for (child = root->left_child; child != NULL; child = child->right_sibling)
    print_ast(child, depth + 1);
}  
