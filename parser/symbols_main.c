/*
 * symbols_main.c    A driver for the pccbr symbol table generator
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
 * Function: build_symboltable
 *  @table  - the symbol table to build
 *  @root   - the root node of the AST
 *  @curr   - the current AST node to add to the table
 *
 * A function to recursively build the symbol table.
 *
 * Disclaimers:
 *  - By necessity, this is a hairy function. There are lots of complicated
 *    cases to check for.
 *
 * PseudoCode:
 *  I.    Catch the type of the ast_node.
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
          insert_into_symboltable(table, INT_LT, child->left_child->value_string, child->left_child->lineno);
        // Node is a valueless declaration => add to symtab
        } else if (child->node_type == DEC_ID_N) {
          if (child->value_int == 0) // Not an array
            insert_into_symboltable(table, INT_LT, child->value_string, child->lineno); 
          else
            insert_into_symboltable(table, INT_ARRAY_LT, child->value_string, child->lineno);
        // Node is a function => add to symtab & recurse
        } else if (child->node_type == FUNC_N) {
          var_lookup_type type = (child->value_int == 0 ? FUNC_VOID_LT : FUNC_INT_LT);
          insert_into_symboltable(table, type, child->value_string, child->lineno);
          build_symboltable(table, root, child);
        // Node is other => recurse
        } else if (child->node_type == ERROR_N && 
            (strcmp(child->value_string, "Invalid Variable Declaration")) == 0) { 
          insert_into_symboltable(table, ERROR_LT, child->value_string, child->lineno);
        } else {
          build_symboltable(table, root, child);
        }
      }
      return table; // Since we didn't enter a new scope, return before exiting
      break;


    case COMPOUND_STMT_N :
      enter_scope(table, NULL);

      // Loop through declarations, adding them to the current level's table
      // This is the nastiest for-loop I've ever written. But it has to happen
      for (child = curr->left_child; 
          
          // This is the nasty conditional on the for-loop:  
          child != NULL &&                      // Child is not null
            (child->node_type == OP_ASSIGN_N || // Child may be a declaration
            child->node_type == DEC_ID_N ||
            (child->node_type == ERROR_N && 
              (strcmp(child->value_string, "Invalid Variable Declaration") == 0)));

          child = child->right_sibling) {
        
        if (child->node_type == OP_ASSIGN_N) {
          if (child->left_child->node_type == DEC_ID_N) {
            insert_into_symboltable(table, INT_LT, child->left_child->value_string, child->left_child->lineno);
          }
        } else if (child->node_type == DEC_ID_N) { 
          if (child->value_int == 0) // Not an array
            insert_into_symboltable(table, INT_LT, child->value_string, child->lineno);
          else
            insert_into_symboltable(table, INT_ARRAY_LT, child->value_string, child->lineno);
        } else {  // Node is an error
          insert_into_symboltable(table, ERROR_LT, child->value_string, child->lineno);
        }
      }
      // Loop through remaining children, recursing on them
      // Child is already set to the first non-declaration item
      for ( ; child != NULL; child = child->right_sibling)
        build_symboltable(table, root, child);
      
      break;


    case FUNC_N :
      enter_scope(table, curr);

      // Loop past the compound statement (function body) and then to the params
      for (child = curr->left_child->right_sibling; child != NULL; child = child->right_sibling) {
        if (child->node_type == PARAM_N) {
          insert_into_symboltable(table, INT_LT, child->value_string, child->lineno);
        } else if (child->node_type == ARRAY_PARAM_N) {
          insert_into_symboltable(table, INT_ARRAY_LT, child->value_string, child->lineno);
        } else {
          continue; // Void => break from the for-loop
        }
      }

      // Recurse on the function body
      build_symboltable(table, root, curr->left_child);
      break;

    default :
      for (child = curr->left_child; child != NULL; child = child->right_sibling)
        build_symboltable(table, root, child);
      return table;
  }

  leave_scope(table);
  return table;
}


/*
 * Check the tree for consistency with the symbol table
 */
int typecheck_ast(symboltable_t *table, ast_node node) {
  ast_node child = node->left_child, rsib = node->right_sibling;
  ast_node curr;
  symnode_t *symbol;
  var_lookup_type group_type;
  int same_scope = 1; // 1 => same scope, 0 => entered deeper scope

  /* Change scope if necessary */
  if (node->node_type == FUNC_N || node->node_type == COMPOUND_STMT_N) {
    /* FIX THIS */
    if (table->leaf->child)
      table->leaf = table->leaf->child;
  }

  /* Recurse on Children */
  if (child)
    typecheck_ast(table, child);

  if (rsib)
    typecheck_ast(table, rsib);

  switch(node->node_type) {
    case ID_N :
      symbol = lookup_in_symboltable(table, node->value_string);
      node->lineno = symbol->lineno;
      node->dtype = symbol->type;
      break;

    // #swag
    case OP_ASSIGN_N ... OP_DECREMENT_N :
      group_type = child->dtype;

      for (curr = child; curr != NULL; curr = curr->right_sibling) {
        if (curr->dtype != group_type) {
          // Make error reporting better?
          fprintf(stderr, "Type Mismatch on line %d\n", node->lineno);
          return -1;
        }
      }

      node->dtype = group_type;

      break;

    case FUNC_CALL_N :

    default:
      break;
  }

  if (same_scope == 0)
    table->leaf = table->leaf->parent;

  return 0;
} 
/*
 * Build the AST and create a Symbol Table for it
 */
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
  int retval = typecheck_ast(symtab, root);
  printf("Symtable created...\n");

  print_symtab(symtab);
  print_checked_ast(stdout, root, 0);

  return 0;

}
