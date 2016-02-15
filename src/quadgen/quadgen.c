#include "quadgen.h"
#include "../util/symtab.h"
#include <string.h>
#include <stdio.h>

#define   MAX_TEMP_COUNT  1000000

extern quad_t list;
quad_t list_end;

static const char *label_prefix = "L_";
static const char *temp_prefix = "T";

static int tempCount = 0;

char *NewLabel(char *nodename, char *text);
char *generate_temp_name();
void print_quad(FILE *fp, quad_t quad);

quad_t create_quad(optype op, symnode_t *operand1, symnode_t *operand2, symnode_t *operand3) {
  quad_t new_quad = calloc(1, sizeof(struct quad_struct));
  new_quad->op = op;
  new_quad->operand1 = operand1;
  new_quad->operand2 = operand2;
  new_quad->operand3 = operand3;

  return new_quad;
}

quad_t add_quad(optype op, symnode_t *operand1, symnode_t *operand2, symnode_t *operand3) {
  quad_t new_quad = create_quad(op, operand1, operand2, operand3);
  list_end->next = new_quad;
  list_end = new_quad;
  return new_quad;
}

symnode_t *code_gen(ast_node node, symboltable_t *table) {
  ast_node child = node->left_child;
  int changed_scope = 0;
  symnode_t *retval = NULL;
  // Basic temps and intermideary values
  symnode_t *t0, *t1, *tx, *ty, *x, *y;
  ast_node iterator;
  char *buff;

  if (node->node_type == FUNC_N || node->node_type == COMPOUND_STMT_N) {
    // Change to next child scope
    change_scope(table);
    changed_scope = 1; 
  }

  switch(node->node_type) {

    // for each child node
      // code_gen(child)
    case ROOT_N :
      list = create_quad(NULL_QOP, NULL, NULL, NULL);
      list_end = list;

      for (iterator = child; iterator != NULL; iterator = iterator->right_sibling)
        retval = code_gen(iterator, table);
      
      break;

    // t0 <- newTemp()
    // rightVar = codeGen(rightChild)
    // quad(assn, t0, rightVar, -)
    // leftVar = codeGen(leftChild)
    // quad(assn, leftVar, t0, -)
    // return leftVar
    case OP_ASSIGN_N :
      t0 = NewTemp(table); 
     
      // we have to treat declaration ID's slightly different
      if (child->node_type == DEC_ID_N && child->right_sibling->node_type == INT_LITERAL_N) {
        // Manually create a value for the untracked constant
        buff = calloc(1, sizeof(char) * 11); // Assumes int <= 10 digits
        buff[10] = '\0';
        sprintf(buff, "%d", child->right_sibling->value_int);
        y = create_symnode(buff, TEMP_LT, NULL, -1);
        buff = NULL;

        add_quad(ASSN_QOP, t0, y, NULL);

        x = code_gen(child, table);
        add_quad(ASSN_QOP, x, t0, NULL);

      } else {
        y = code_gen(child->right_sibling, table);
        add_quad(ASSN_QOP, t0, y, NULL);
      
        x = code_gen(child, table);
        add_quad(ASSN_QOP, x, t0, NULL);
      }
      retval = x;
      break;

    // leftVar = codeGen(leftChild)
    // rightVar = codeGen(rightChild)
    // t0 = newTemp()
    // quad(add, t0, leftVar, rightVar) 
    // return t0
    case OP_PLUS_N :
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);      
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);

      add_quad(ADD_QOP, t0, tx, ty);

      retval = t0;
      break; 

    case OP_MINUS_N :
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);      
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);

      add_quad(SUB_QOP, t0, tx, ty);

      retval = t0;
      break;      // Same as plus

    case OP_TIMES_N :
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);      
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);

      add_quad(MULL_QOP, t0, tx, ty);

      retval = t0;
      break;      // Same as plus
    

    case OP_DIVIDE_N :
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);      
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);

      add_quad(DIV_QOP, t0, tx, ty);

      retval = t0;
      break;      // Same as plus


    case OP_MOD_N :
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);      
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);

      add_quad(MOD_QOP, t0, tx, ty);

      retval = t0;
      break;      // Same as plus

    // x = codeGen(leftChild)
    // t0 = newTemp()
    // t1 = newTemp()
    // quad(load, t1, 0, -);
    // quad(sub, t0, t1, leftVar)
    // return t0
    case OP_NEG_N :
      t0 = NewTemp(table);
      tx = NewTemp(table);

      // Manually create a 1-time use symnode for the value 0
      /* WARNING: This may cause problems down the road!! */
      t1 = create_symnode("0", TEMP_LT, NULL, -1);

      // Assignment (for registers)
      x = code_gen(child, table);
      add_quad(ASSN_QOP, tx, x, NULL);
      
      // Negation
      add_quad(SUB_QOP, t0, t1, tx);

      retval = t0;
      break;

    case OP_UNARYM_N :
      t0 = NewTemp(table);
      tx = NewTemp(table);

      // Manually create a 1-time use symnode for the value 0
      /* WARNING: This may cause problems down the road!! */
      t1 = create_symnode("0", TEMP_LT, NULL, -1);

      // Assignment (for registers)
      x = code_gen(child, table);
      add_quad(ASSN_QOP, tx, x, NULL);
      
      // Negation
      add_quad(SUB_QOP, t0, t1, tx);

      retval = t0;
      break;


    /* 
     * ==================================================
     *        Boolean Nodes
     * ==================================================
     */
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
    

    case INT_LITERAL_N :
      buff = calloc(1, sizeof(char) * 11); // Assumes int <= 10 digits
      buff[10] = '\0';
      sprintf(buff, "%d", node->value_int);
 
      retval = lookup_in_symboltable(table, buff, CONST_VT);

      if (retval == NULL) {
        retval = create_symnode(buff, TEMP_LT, NULL, -1);
        buff = NULL;
      } 
      break;

    case ID_N :
      retval = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
      break;

    case DEC_ID_N :
      retval = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
      break;

    default :
      retval = NULL;
  } // End of Case Statement

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

  case IF_N:
    // x = codeGen(leftChild)
    // L1 = label("END_IF")
    // (ifFalse, x, L1, -)
    // codeGen(rightChild)
    // (label, L1, -, -)

  case IF_ELSE_N:
    // x = codeGen(leftChild)
    // L1 = newlabel("ELSE")
    // L2 = newLabel("END_IF")
    // (ifFalse, x, L1, -)
    // codeGen(second_child)
    // (goto, L2, -, -)
    // (label, L1, -, -)
    // codeGen(third_child)
    // (label, L2, -, -)

  case PRINT_N:
    // x = codeGen(leftChild)
    // (print, x, -, -)

  case READ_N:
    // NOTE: This might need more thinking through cause we're probably expecting to write to an
    // address. In this conception t0 returns the address of the string stored in memory.
    
    // t0 = newTemp()
    // (read, t0, -, -)
    // return t0

  case RETURN_N:
    // t0 = newTemp()
    // x = codeGen(leftChild)
    // (assn, t0, x, -)
    // (return, t0, -, -)

  case FOR_N:
    // x = codeGen(leftChild)
    // L1 = newLable("COND")
    // L2 = newLable("END_FOR")
    // (label, L1, -, -)
    // y = codeGen(secondChild)
    // (ifFalse, y, L2, -)
    // codeGen(fourthChild)
    // codeGen(thirdChild)
    // (goto, L1, -, -)
    // (label, L2, -, -)

  case WHILE_N:
    // L1 = newLabel("COND")
    // L2 = newLabel("END_WHILE")
    // label(L1, -, -, -)
    // x = codeGen(leftChild)
    // (ifFalse, x, L2, -)
    // codeGen(rightChild)
    // (goto, L1, -, -)
    // label(L2, -, -, -)

  case DO_WHILE_N:
    // L1 = newLabel("START")
    // (label, L1, -, -)
    // codeGen(rightChild)
    // x = codeGen(leftChild)
    // (ifTrue, x, L1, -) 

  case DO_WHILE_N:
    // L1 = newLabel("START")
    // (label, L1, -, -)
    // codeGen(rightChild)
    // x = codeGen(leftChild)
    // (ifTrue, x, L1, -)    

  case COMPOUND_STATEMENT_N:
    // for each child
      // codeGen(child)

  case FUNC_N:
    // L1 = newLabel(node->value_string -- should be func name --)
    // (label, L1, -, -)
    // child = first Compound statement child
    // codeGen(child)

  case ID_N:
    // t0 = newTemp()
    // if is an array (check node->value_int)
      // x = codeGen(child)
      // y = lookupInSymtab()
      // (index, t0, y, x)
    // else
      // (assn, t0, lookUpInSymtab(), -)
    // return t0

  if (changed_scope) {
    table->leaf = table->leaf->parent;
  }

  printf("%s\n", NewLabel(node->node_name, LABEL_NODE_NAME(node->node_type)));
  return retval;
}

/* 
 * Creates a new temporary variable in the current scope of the program
 */
symnode_t *NewTemp(symboltable_t *table) {
  symnode_t *temp;
  char *name = generate_temp_name();
  // Notice that the line number doesn't apply here 
  temp = insert_temp(table, TEMP_LT, name, 0); 
  return temp;
}

/*
 * Pretty-prints a quad list
 */
void print_quad_list(FILE *fp, quad_t list) {
  assert(list);
  quad_t it = NULL;
  for (it = list; it != NULL; it = it->next)
    print_quad(fp, it);

}

void print_quad(FILE *fp, quad_t quad) {
  assert(quad);
  fprintf(fp, "(%d, ", quad->op);
  if (quad->operand1)
    fprintf(fp, "%s, ", quad->operand1->name);
  else
    fprintf(fp, "-, ");

  if (quad->operand2)
    fprintf(fp, "%s, ", quad->operand2->name);
  else
    fprintf(fp, "-, ");

  if (quad->operand3)
    fprintf(fp, "%s)\n", quad->operand3->name);
  else
    fprintf(fp, "-)\n");
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
