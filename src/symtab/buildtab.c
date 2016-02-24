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
#include "buildtab.h"
//#include "../quadgen/quadgen.h" // For testing temp generation

symboltable_t *build_symboltable(symboltable_t *table, ast_node root, ast_node curr) {
  ast_node it;
  ast_node child, grandchild;
  symnode_t *param_entry;
  int param_count = 0;
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
      curr->scope = table->leaf;

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
      curr->scope = table->leaf;

      child = curr->left_child;
      if (child->node_type == COMPOUND_STMT_N) {
        for (grandchild = child->left_child; grandchild->right_sibling != NULL; grandchild = grandchild->right_sibling);
        if (grandchild->node_type != RETURN_N)
          grandchild->right_sibling = create_ast_node(RETURN_N);
      }

      // Count the number of parameters
      param_count = 0;
      for (child = curr->left_child->right_sibling; child != NULL; child = child->right_sibling)
        param_count++;

      // Loop past the compound statement (function body) and then to the params
      for (child = curr->left_child->right_sibling; child != NULL; child = child->right_sibling) {
        if (child->node_type == PARAM_N) {
          param_entry = insert_into_symboltable(table, INT_LT, child->value_string, child->lineno);
          param_entry->mem_location = param_count * 4;
          param_count--;
          param_entry->absolute_address = 0;
        } else if (child->node_type == ARRAY_PARAM_N) {
          param_entry = insert_into_symboltable(table, INT_ARRAY_LT, child->value_string, child->lineno);
          param_entry->mem_location = param_count * 4;
          param_count--;
          param_entry->absolute_address = 0;
        } else {
          continue; // Void => break from the for-loop
        }
      }

      // Recurse on the function body
      build_symboltable(table, root, curr->left_child);
      break;

    default :
      if (curr->node_type == STRING_LITERAL_N) {
        insert_constant(table, STR_LT, curr->value_string, curr->lineno);
      }

      if (curr->node_type == INT_LITERAL_N) {
        // ASSUMES Integers can be only up to 10 digits long
        char *name_buf = calloc(1, sizeof(char) * 11);
        name_buf[10] = '\0';
        sprintf(name_buf, "%d", curr->value_int);
        insert_constant(table, INT_LT, name_buf, curr->lineno);
      }

      for (child = curr->left_child; child != NULL; child = child->right_sibling)
        build_symboltable(table, root, child);
      return table;
  }

  leave_scope(table);
  return table;
}
