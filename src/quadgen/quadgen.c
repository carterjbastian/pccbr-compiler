#include "quadgen.h"
#include "../util/symtab.h"
#include <string.h>
#include <stdio.h>

#define   MAX_TEMP_COUNT  1000000


static const char *label_prefix = "L_";
static const char *temp_prefix = "T";

static int tempCount = 0;

char *NewLabel(char *nodename, char *text);
char *generate_temp_name();

quad_t create_quad(optype op, symnode_t *operand1, symnode_t *operand2, symnode_t *operand3) {
  quad_t new_quad = calloc(1, sizeof(struct quad_struct));
  new_quad->op = op;
  new_quad->operand1 = operand1;
  new_quad->operand2 = operand2;
  new_quad->operand3 = operand3;

  return new_quad;
}

symnode_t *code_gen(ast_node node, symboltable_t *table, quad_t list) {
  ast_node child = node->left_child, rsib = node->right_sibling;
  int changed_scope = 0;

  if (node->node_type == FUNC_N || node->node_type == COMPOUND_STMT_N) {
    // Change to next child scope
    change_scope(table);
    changed_scope = 1; 
  }

  switch(node->node_type) {
    case ROOT_N {
      // for each child node
        // code_gen(child)
      break;
    }

    case OP_ASSIGN_N {
      // t0 <- newTemp()
      // rightVar = codeGen(rightChild)
      // quad(assn, t0, rightVar, -)
      // leftVar = codeGen(leftChild)
      // quad(assn, leftVar, t0, -)
      // addQuad
      // return leftVar
      break;
    }

    case OP_PLUS_N {
      // leftVar = codeGen(leftChild)
      // rightVar = codeGen(rightChild)
      // t0 = newTemp()
      // quad(add, t0, leftVar, rightVar) 
      // return t0
    }

    case OP_MINUS_N {
      // Same as plus
    }

    case OP_TIMES_N {
      // Same as plus
    }

    case OP_DIV_N {
      // Same as minus
    }

    case OP_TIMES_N {
      // Same as div
    }

    case OP_NEG_N {
      // x = codeGen(leftChild)
      // t0 = newTemp()
      // t1 = newTemp()
      // quad(load, t1, 0, -);
      // quad(sub, t0, t1, leftVar)
      // return t0
    }

    case OP_MOD
      // Same as divide
    
    case OP_LT_N:
      // x = codeGen(leftChild)
      // y = codeGen(rightChild)
      // t0, t1 = newTemp()
      // t2 = newTemp()
      // (lt, t2, x, y) 
      // return t2

    case OP_LTE_N:
      // same w/ LTE_QOP
    
    case OP_EQUALS_N:
      // same w/ EQ_QOP

    case OP_GT_N:
      // same w/ EQ_QOP

    case OP_GTE_N:
      // same w/ GTE_QOP

    case OP_AND_N:
      // t0 = newTemp()
      // L1 = newLabel(node->node_name, "IS_FALSE")
      // L2 = newLabel(node->node_name, "END")
      // x = codeGen(leftChild)
      // (ifFalse, x, L1, -)
      // y = codeGen(rightChild)
      // (ifFalse, y, L1, -)
      // (assn, t0, 1, -)
      // (goto, L2, -, -)
      // (label, L1, -, -)
      // (assn, t0, 0, -)
      // (label, l2, -, -)
      // return t0
  }
  
  case OP_OR_N:
      // t0 = newTemp()
      // L1 = newLabel(node->node_name, "IS_TRUE")
      // L2 = newLabel(node->node_name, "END")
      // x = codeGen(leftChild)
      // (ifTrue, x, L1, -)
      // y = codeGen(rightChild)
      // (ifTrue, y, L1, -)
      // (assn, t0, 0, -)
      // (goto, L2, -, -)
      // (label, L1, -, -)
      // (assn, t0, 1, -)
      // (label, l2, -, -)
      // return t0
  }

  case OP_COMPLEMENT_N:
    // t0 = newTemp()
    // L1 = newLabel("IS_FALSE")
    // L2 = newLabel(end)
    // x = codeGen(leftChild)
    // (ifFalse, x, L1, -)
    // (assn, t0, 0, -)
    // (goto, L2, -, -)
    // (label, L1, -, -)
    // (assn, t0, 1, -)
    // (label, l2, -, -)
    // return t0

  case OP_INCREMENT_N:
    // t0, t1 = newTemp()
    // x = codeGen(leftChild)
    // (assn, t1, 1, 0)
    // (add, t0, x, 1)
    // return t0

  case OP_DECREMENT_N:
    // Same as increment

  case FUNC_CALL_N:
    // For each arg_node (should be all children unless child is no_formal_params)
      // t0 = newTemp()
      // t0 = codeGen(arg_node)
      // (arg, t, -, -)
    // func_call = look_up_symnode(node->value_string) -- need to find node for current value so we can reference function
    // (pre_call, func_call, -, -)
    // if (func_returns) 
      // t1 = newTemp()
    // else
      // t1 = NULL

    // (post_return, t, func_call, -)
    // return t

  if (changed_scope) {
    table->leaf = table->leaf->parent;
  }

  printf("%s\n", NewLabel(node->node_name, LABEL_NODE_NAME(node->node_type)));
  return NULL;
}

/* 
 * Creates a new temporary variable in the current scope of the program
 */
symnode_t *NewTemp(symboltable_t *table, var_lookup_type type) {
  symnode_t *temp;
  char *name = generate_temp_name();
  // Notice that the line number doesn't apply here 
  temp = insert_temp(table, type, name, 0); 
  return temp;
}

char *generate_temp_name() {
  assert(tempCount < MAX_TEMP_COUNT);

  char *tName = calloc(1, sizeof(char) * 8);
  assert(tName);
  tName[7] = '\0';
  sprintf(tName, "%s%d", temp_prefix, tempCount);
  tempCount++;
  return tName;
}

char *NewLabel(char *nodename, char *text) {
  int l1 = strlen(nodename);
  int l2 = strlen(text);

  // string = "L_" + nodename + "_" + text + '\0'
  int len = 2 + l1 + 1 + l2 + 1;

  char *label = calloc(1, sizeof(char) * len);
  sprintf(label, "%s%s_%s", label_prefix, nodename, text);
  return label;
}
