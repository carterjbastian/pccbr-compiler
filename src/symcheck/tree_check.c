#include "tree_check.h"
#include <string.h>

symhashtable_t *find_scope_for_function(symhashtable_t *parent_scope, char *func_name) {
  symhashtable_t *curr;

  for (curr = parent_scope->child; curr != NULL; curr = curr->rightsib) {
        if (curr->declaring_func == NULL)
          continue;

        if (strcmp(curr->declaring_func->value_string, func_name) == 0)
          break;
      }

  return curr;
}

/*
 * Check the tree for consistency with the symbol table
 */
int typecheck_ast(symboltable_t *table, ast_node node) {
  ast_node child = node->left_child, rsib = node->right_sibling;
  ast_node curr, parent_curr;
  symnode_t *parent_symbol;
  symnode_t *symbol;
  symhashtable_t *parent, *func_scope;
  var_lookup_type group_type;
  int changed_scope = 0; // 0 => same scope, 1 => entered deeper scope
  int arg_count = 0;
  int ret_val = 0;

  /* Change scope if necessary */
  if (node->node_type == FUNC_N || node->node_type == COMPOUND_STMT_N) {
    // Change to next child scope
    change_scope(table);
    changed_scope = 1; 
  }

  /* Recurse on Children */
  if (child)
    ret_val += typecheck_ast(table, child);
  
  if (changed_scope) {
    if (table->leaf->parent)
      table->leaf = table->leaf->parent;
  }

  if (rsib)
    ret_val += typecheck_ast(table, rsib);

  switch(node->node_type) {
    case ID_N :
      symbol = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
      if (!symbol) {
        fprintf(stderr, "Using symbol %s on line %d before declaration\n", node->value_string, node->lineno);
        ret_val += 1;
        break;
      }
      node->lineno = symbol->lineno;
      node->dtype = symbol->type;
      break;

    // #swag
    case OP_ASSIGN_N:
      if (child->node_type != ID_N && child->node_type != DEC_ID_N) {
        fprintf(stderr, "Attempting to assign to invalid expression %s on line %d\n", child->value_string, node->lineno);
        node->dtype = ERROR_LT;
        ret_val += 1;
        break;
      }

    case OP_PLUS_N ... OP_DECREMENT_N :
      group_type = child->dtype;

      for (curr = child; curr != NULL; curr = curr->right_sibling) {
        if (curr->node_type == OP_ASSIGN_N) {
          fprintf(stderr, "Error: expressionis not assignable on line %d\n", node->lineno);
          ret_val += 1;
        }
        if (curr->dtype != group_type) {
          // Make error reporting better?
          fprintf(stderr, "Type Mismatch in operation on line %d\n", node->lineno);
          ret_val += 1;
        }
      }

      node->dtype = group_type;

      break;

    case FUNC_CALL_N :
      // Check to see if the current symbol is valid
      symbol = lookup_in_symboltable(table, node->value_string, LOCAL_VT);

      if (!symbol || (symbol->type != FUNC_INT_LT && symbol->type != FUNC_VOID_LT))  {
          fprintf(stderr, "Symbol %s is not a function on line %d\n", node->value_string, node->lineno);
          return ret_val + 1;
      }
      
      // The hash table containing the parent nodes
      parent = symbol->parent;

      func_scope = find_scope_for_function(parent, node->value_string);

      for (parent_curr = func_scope->declaring_func->left_child;
          parent_curr != NULL && parent_curr->node_type != PARAM_N && parent_curr->node_type != ARRAY_PARAM_N;
          parent_curr = parent_curr->right_sibling);
            
      for (curr = child; curr != NULL; curr = curr->right_sibling) {
        // Check types and count of children

        if (parent_curr == NULL) {
            if (curr->node_type == NULL_N) break;

          fprintf(stderr, "Too many args in call to %s on line %d\n", node->value_string, node->lineno);
          ret_val += 1;
          break;
        }
        
        if (curr->node_type == NULL_N) {
          fprintf(stderr, "Too few args in call to %s on line %d\n", node->value_string, node->lineno);
          ret_val += 1;
          break;
        }

        if (curr->dtype == INT_LT && parent_curr->node_type != PARAM_N) {
          fprintf(stderr, "Type Mismatch on line %d\n", node->lineno);
          ret_val += 1;
          break;
        }

        if (curr->dtype == INT_ARRAY_LT && parent_curr->node_type != ARRAY_PARAM_N) {
          fprintf(stderr, "Type Mismatch on line %d\n", node->lineno);
          ret_val += 1;
          break;
        }

        parent_curr = parent_curr->right_sibling;
      }

      if (parent_curr != NULL) {
          fprintf(stderr, "Too few args in call to %s on line %d\n", symbol->name, node->lineno);
          ret_val += 1;
      }
      
      if (symbol->type == FUNC_INT_LT) {
        node->dtype = INT_LT;
      } else {
        node->dtype = FUNC_VOID_LT;
      }

      break;

    case RETURN_N :
      for (func_scope = table->leaf;
        func_scope != NULL && func_scope->declaring_func == NULL;
        func_scope = func_scope->parent);
      
      if (func_scope == NULL) {
        fprintf(stderr, "Too many arguments in call to %s on line %d\n", symbol->name, node->lineno);
        ret_val += 1;
      }

      if (func_scope->declaring_func->value_int == 1) {
        if(child == NULL) {
          fprintf(stderr, "Void return in function %s that should return int on line %d\n", func_scope->declaring_func->value_string, node->lineno);
          ret_val += 1;
        }

        else if(child->dtype != INT_LT) {
          if (!(child ->dtype == INT_ARRAY_LT && child->left_child != NULL)) {
            fprintf(stderr, "Bad return in function that should return int on line %d\n", node->lineno);
            ret_val += 1;
          }
        }

        node->dtype = INT_LT;
      }

      if (func_scope->declaring_func->value_int == 0) {
        if(child != NULL) {
          fprintf(stderr, "Returning value in function that should return void on line %d\n", node->lineno);
          ret_val += 1;
        }
        
        node->dtype = VOID_LT;
      }

      node->returns_to = func_scope->declaring_func;
      break;

    default:
      break;
  }

  return ret_val;
} 


