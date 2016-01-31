/*
 * tree_main.c    A driver to print or test the output of the pccbr parser
 *
 * Authors:
 *  Originally written by THC for CS 57 and massaged by SWS.
 *  Extended for use in the pccbr-compiler project by Carter J. Bastian
 *    and Quinn Stearns in 2016.
 *
 * This File Contains:
 *  - Driver program for a parser that builds an abstract syntax tree
 *  - A testing suite for the parser to demonstrate that it works
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "symtab.h"

/* Only include info needed for autoTesting if in Test Mode */
#ifdef _TESTING_
#include "testfiles/autoTest.h"
#endif

ast_node root = NULL;

extern int yyparse();
extern int yydebug;
extern FILE *yyin;

// Used in the parser (parser.y) for error recovery and reporting.
int parseError = 0;
FILE *error_out;  // Global conditionally used in testing mode only

/*
 * A function to recursively build the symbol table.
 *
 * PseudoCode:
 *  I.    Catch irrelevant ast_nodes
 *  II.   Enter a new scope (for the current ast_node)
 *  II.   Loop through (Based on the type of ast_node):
 *      a. Add the newly-declared variables to the new scope
 *      b. Recurse on each sibling node in the array
 *  IV.   Exit scope and return symbol table so far
 */
symboltable_t *build_symboltable(symboltable_t *table, ast_node root, ast_node curr) {
  ast_node it;
  ast_node child;  
  /*
   * Use this switch to parse each type of node correctly
   */
  switch(curr->node_type) {
    case ROOT_N :

      // Loop to the right until you get to null
      for (child = curr->left_child; child != NULL; child = child->right_sibling) {
        // The node is a declaration => add to symtab
        if (child->node_type == OP_ASSIGN_N) {
          insert_into_symboltable(table, INT_LT, child->left_child->value_string);
        // Node is a valueless declaration => add to symtab
        } else if (child->node_type == ID_N) {
          if (child->left_child == NULL) // Not an array
            insert_into_symboltable(table, INT_LT, child->value_string); 
          else
            insert_into_symboltable(table, INT_ARRAY_LT, child->value_string);
        // Node is a function => add to symtab & recurse
        } else if (child->node_type == FUNC_N) {
          var_lookup_type type = (child->value_int == 0 ? FUNC_VOID_LT : FUNC_INT_LT);
          insert_into_symboltable(table, type, child->value_string);
          build_symboltable(table, root, child);
        // Node is other => recurse
        } else if (child->node_type == ERROR_N && 
            (strcmp(child->value_string, "Invalid Variable Declaration")) == 0) { 
          insert_into_symboltable(table, ERROR_LT, child->value_string);
        } else {
          build_symboltable(table, root, child);
        }
      }
      return table; // Since we didn't enter a new scope, return before exiting
      break;


    case COMPOUND_STMT_N :
      enter_scope(table);

      // Loop through declarations, adding them to the current level's table
      // This is the nastiest for-loop I've ever written. But it has to happen
      for (child = curr->left_child; 
          // This is the nasty conditional on the for-loop:  
          child != NULL &&                      // Child is not null
            (child->node_type == OP_ASSIGN_N || // Child may be a declaration
            child->node_type == ID_N ||
            (child->node_type == ERROR_N && 
              (strcmp(child->value_string, "Invalid Variable Declaration") == 0)));
          child = child->right_sibling) {
        
        if (child->node_type == OP_ASSIGN_N) {
          insert_into_symboltable(table, INT_LT, child->left_child->value_string);
        } else if (child->node_type == ID_N) { 
          if (child->left_child == NULL) // Not an array
            insert_into_symboltable(table, INT_LT, child->value_string);
          else
            insert_into_symboltable(table, INT_ARRAY_LT, child->value_string);
        } else {  // Node is an error
          insert_into_symboltable(table, ERROR_LT, child->value_string);
        }
      }
      // Loop through remaining children, recursing on them
      // Child is already set to the first non-declaration item
      for ( ; child != NULL; child = child->right_sibling)
        build_symboltable(table, root, child);
      
      break;


    case FUNC_N :
      enter_scope(table);

      // Loop past the compound statement (function body) and then to the params
      for (child = curr->left_child->right_sibling; child != NULL; child = child->right_sibling) {
        if (child->node_type == PARAM_N) {
          insert_into_symboltable(table, INT_LT, child->value_string);
        } else if (child->node_type == ARRAY_PARAM_N) {
          insert_into_symboltable(table, INT_ARRAY_LT, child->value_string);
        } else {
          continue; // Void => break from the for-loop
        }
      }

      // Recurse on the function body
      build_symboltable(table, root, curr->left_child);
      break;

    default :
      return table;
  }

  leave_scope(table);
  return table;
}

int main() {
  int noRoot = 0;		/* 0 means we will have a root */
  symboltable_t *symtab;

  /* Build the tree */
  error_out = stderr;
  noRoot = yyparse();

  if (parseError && (!noRoot))
    fprintf(stderr, "WARNING: There were %d parse errors.\nParse tree may be ill-formed.\n", parseError);

  if (noRoot)
    fprintf(stderr, "Parsing reached fatal error. No AST was constructed\n");

  /* Set up the symbol tree */
  symtab = create_symboltable();
  symtab = build_symboltable(symtab, root, root);
  printf("Symtable created...\n");

  print_symtab(symtab);

  return 0;

}
