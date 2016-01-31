/* 
 * ast.c    A file implementing the functions to work with the ast_node datatype
 * 
 * Authors:
 *  Originally Written by THC for CS 57 and massaged by SWS
 *  Extended for use in the pccbr-compiler by Carter J. Bastian and
 *  Quinn Stearns in 2016.
 *
 * This file contains: 
 *  - functions to create a node and to print out an abstract syntax tree.
 *
 * Special Considerations:
 */

// System Includes:
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Local Includes:
#include "ast.h"

// Public Function Definitions:
/* 
 * Function: create_ast_node
 *  @node_type - the type of node to be created (ast_node_type defined in ast.h)
 *  
 * Creates a node with a given token type and return a pointer to the
 * node.
 */
ast_node create_ast_node(ast_node_type node_type) {
  ast_node new_node = calloc(1,sizeof(struct ast_node_struct));  // for zeros
  new_node->node_type = node_type;
  return new_node;
}

/* 
 * Function: print_ast
 *  @root   - the root of the current subtree of an AST
 *  @depth  - the current overall depth into the AST
 *
 * Recursively prints the contents of a subtree of an abstract syntax tree, 
 * when provided with the root of the subtree and the 
 * depth of the subtree root. 
 */
void print_ast(FILE *fp, ast_node root, int depth) {
  /* Print two spaces for every level of depth. */
  int i;

  assert(root);
  
  /* Syntactic sugar */
  fprintf(fp, "| ");
  for (i = 1; i < depth; i++)
    fprintf(fp, "- ");

  /* Print the node type. */
  fprintf(fp, "%s ", NODE_NAME(root->node_type));

  /* Print attributes specific to node types. */
  switch (root->node_type) {
  case ID_N:			/* print the id */
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);
    break;

  case INT_LITERAL_N:		/* print the int literal */
    fprintf(fp, "%d", root->value_int);
    break;

  case STRING_LITERAL_N:        /* print the value of the string literal */
    assert(root->value_string);
    fprintf(fp, "\"%s\"", root->value_string);
    break;

  case PARAM_N:                 /* Print the name of the param */
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);
    break;

  case ARRAY_PARAM_N:           /* Print the name of the array parameter */
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);
    break;

  case FUNC_CALL_N:             /* Print the name of the function being called */
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);
    break;

  case NULL_N:                  /* Print the syntactic purpose of the null node */
    fprintf(fp, "\t%s", root->value_string);
    break;

  case FUNC_N:                  /* Print the name of the function */
    fprintf(fp, "%s", root->value_string);
    break;

  case ERROR_N:                 /* Print the type of error represented */
    assert(root->value_string);
    fprintf(fp, "\t%s", root->value_string);
    break;

  default:
    break;
  }

  fprintf(fp, "\n");

  /* Recurse on each child of the subtree root, with a depth one
     greater than the root's depth. */
  ast_node child;
  for (child = root->left_child; child != NULL; child = child->right_sibling)
    print_ast(fp, child, depth + 1);
}  
