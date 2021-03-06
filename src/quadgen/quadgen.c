#include "quadgen.h"
#include "../util/symtab.h"
#include <string.h>
#include <stdio.h>

#define   MAX_TEMP_COUNT  1000000

extern quad_t list;
quad_t list_end;


static const char *label_prefix = "L_";
static const char *temp_prefix = "T";

int tempCount = 0;

//symnode_t *NewLabel(char *nodename, char *text);
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
  if (node == NULL)
    return NULL;
  ast_node child = node->left_child;
  int changed_scope = 0;
  int count = 0;
  symnode_t *retval = NULL;
  // Basic temps and intermideary values
  symnode_t *t0, *t1, *t2, *tx, *ty, *x, *y, *l1, *l2, *x2;
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
      t1 = NewTemp(table);
       
      // we have to treat declaration ID's slightly different
      if (child->node_type == DEC_ID_N && child->right_sibling->node_type == INT_LITERAL_N) {
        // Manually create a value for the untracked constant
        buff = calloc(1, sizeof(char) * 11); // Assumes int <= 10 digits
        buff[10] = '\0';
        sprintf(buff, "%d", child->right_sibling->value_int);
        y = create_symnode(buff, TEMP_LT, NULL, -1);
        y->hasVal = 1;
        y->val = child->right_sibling->value_int;
        y->mem_location = 0;
        buff = NULL;

        add_quad(ASSN_QOP, t0, y, NULL);

        //x = code_gen(child, table);
        x = lookup_in_symboltable(table, child->value_string, LOCAL_VT);
        add_quad(ASSN_QOP, x, t0, NULL);

        // Special Case of Assigning to an index in an array
      } else if (child->node_type == ID_N && child->value_int != 0) {
        x = lookup_in_symboltable(table, child->value_string, LOCAL_VT); // Will this work with globals?
        t0 = code_gen(child->left_child, table);

        y = code_gen(child->right_sibling, table); // What we're loading into the memory

        add_quad(INDEX_ASSN_QOP, x, t0, y); // Load the memory address of x[t0] into the mem_location of temp1
        
        // Will the retval of x cause problems?? 
      } else {
        y = code_gen(child->right_sibling, table);
        add_quad(ASSN_QOP, t0, y, NULL);
     
        //x = code_gen(child, table);
        x = lookup_in_symboltable(table, child->value_string, LOCAL_VT);
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
      add_quad(ASSN_QOP, x, t0, NULL);
      retval = x;
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
      add_quad(ASSN_QOP, x, t0, NULL);

      retval = 0;
      break;


    /* 
     * ==================================================
     *        Boolean Nodes
     * ==================================================
     */

    // x = codeGen(leftChild)
    // y = codeGen(rightChild)
    // (lt, t2, x, y) 
    // return t2
    case OP_LT_N:
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);
      
      add_quad(LT_QOP, t0, tx, ty);

      retval = t0;
      break;

    case OP_LTE_N:
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);
      
      add_quad(LTE_QOP, t0, tx, ty);

      retval = t0;
      break;
    
    case OP_EQUALS_N:
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);
      
      add_quad(EQ_QOP, t0, tx, ty);

      retval = t0;
      break;
      // same w/ EQ_QOP

    case OP_GT_N:
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);
      
      add_quad(GT_QOP, t0, tx, ty);

      retval = t0;
      break;
      // same w/ EQ_QOP

    case OP_GTE_N:
      t0 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);

      x = code_gen(child, table);
      add_quad(ASSN_QOP, tx, x, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);
      
      add_quad(GTE_QOP, t0, tx, ty);

      retval = t0;
      break;
      // same w/ GTE_QOP

    case OP_AND_N:
      //t0 = newTemp()
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
      t0 = NewTemp(table);

      t1 = create_symnode("0", TEMP_LT, NULL, -1);
      t1->hasVal = 1;
      t1->val = 0;

      t2 = create_symnode("1", TEMP_LT, NULL, -1);
      t2->hasVal = 1;
      t2->val = 1;

      l1 = NewLabel(node->node_name, "IS_FALSE");
      l2 = NewLabel(node->node_name, "END");

      x = code_gen(child, table);
      add_quad(IF_FALSE_QOP, x, l1, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(IF_FALSE_QOP, y, l1, NULL);

      add_quad(ASSN_QOP, t0, t2, NULL);
      add_quad(GOTO_QOP, l2, NULL, NULL);
      add_quad(LABEL_QOP, l1, NULL, NULL);
      add_quad(ASSN_QOP, t0, t1, NULL);
      add_quad(LABEL_QOP, l2, NULL, NULL);

      retval = t0;
      break;


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
      t0 = NewTemp(table);

      t1 = create_symnode("0", TEMP_LT, NULL, -1);
      t1->hasVal = 1;
      t1->val = 0;

      t2 = create_symnode("1", TEMP_LT, NULL, -1);
      t2->hasVal = 1;
      t2->val = 1;

      l1 = NewLabel(node->node_name, "IS_TRUE");
      l2 = NewLabel(node->node_name, "END");

      x = code_gen(child, table);
      add_quad(IF_TRUE_QOP, x, l1, NULL);

      y = code_gen(child->right_sibling, table);
      add_quad(IF_TRUE_QOP, y, l1, NULL);
    
      add_quad(ASSN_QOP, t0, t1, NULL);
      add_quad(GOTO_QOP, l2, NULL, NULL);
      add_quad(LABEL_QOP, l1, NULL, NULL);
      add_quad(ASSN_QOP, t0, t2, NULL);
      add_quad(LABEL_QOP, l2, NULL, NULL);

      retval = t0;
      break;

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
      t0 = NewTemp(table);

      t1 = create_symnode("0", TEMP_LT, NULL, -1);
      t1->hasVal = 1;
      t1->val = 0;

      t2 = create_symnode("1", TEMP_LT, NULL, -1);
      t2->hasVal = 1;
      t2->val = 1;

      l1 = NewLabel(node->node_name, "IS_FALSE");
      l2 = NewLabel(node->node_name, "END");

      x = code_gen(child, table);
      add_quad(IF_FALSE_QOP, x, l1, NULL);

      add_quad(ASSN_QOP, t0, t1, NULL);
      add_quad(GOTO_QOP, l2, NULL, NULL);
      add_quad(LABEL_QOP, l1, NULL, NULL);
      add_quad(ASSN_QOP, t0, t2, NULL);
      add_quad(LABEL_QOP, l2, NULL, NULL);

      retval = t0;
      break;


  case OP_INCREMENT_N:
    // t0, t1 = newTemp()
    // x = codeGen(leftChild)
    // (assn, t1, 1, 0)
    // (add, t0, x, 1)
    // return t0


    if (!(child->node_type == ID_N && child->left_child != NULL)) {      
      t0 = NewTemp(table);
      t1 = NewTemp(table);
      t2 = create_symnode("1", TEMP_LT, NULL, -1);
      t2->hasVal = 1;
      t2->val = 1;

      x = code_gen(child, table);

      add_quad(ASSN_QOP, t1, t2, NULL);
      add_quad(ADD_QOP, t0, x, t1);
      add_quad(ASSN_QOP, x, t0, NULL);
      retval = x;  

    } else {
      t0 = NewTemp(table);
      x = lookup_in_symboltable(table, child->value_string, LOCAL_VT); // Will this work with globals?
      t0 = code_gen(child->left_child, table);

      y = code_gen(child->right_sibling, table); // What we're loading into the memory

        
      // Will the retval of x cause problems?? 

      /*
      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);
      */
      t1 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);
      x2 = NewTemp(table);
      add_quad(NULL_QOP, NULL, NULL, NULL);
//      x2 = code_gen(child, table);      
      add_quad(INDEX_QOP, x2, x, t0);
      add_quad(ASSN_QOP, tx, x2, NULL);
      add_quad(NULL_QOP, NULL, NULL, NULL);

      t2 = create_symnode("1", TEMP_LT, NULL, -1);
      t2->hasVal = 1;
      t2->val = 1;
      add_quad(ASSN_QOP, ty, t2, NULL);

      add_quad(ADD_QOP, t1, tx, ty);

      
      /* FINISH */

      add_quad(INDEX_ASSN_QOP, x, t0, t1); // Load the memory address of x[t0] into the mem_location of temp1
    }
      
    break;

  case OP_DECREMENT_N:
    if (!(child->node_type == ID_N && child->left_child != NULL)) {      
      t0 = NewTemp(table);
      t1 = NewTemp(table);
      t2 = create_symnode("1", TEMP_LT, NULL, -1);
      t2->hasVal = 1;
      t2->val = 1;

      x = code_gen(child, table);
      
      add_quad(ASSN_QOP, t1, t2, NULL);
      add_quad(SUB_QOP, t0, x, t1);
      add_quad(ASSN_QOP, x, t0, NULL);

      retval = x;
    } else {
      t0 = NewTemp(table);
      x = lookup_in_symboltable(table, child->value_string, LOCAL_VT); // Will this work with globals?
      t0 = code_gen(child->left_child, table);

      y = code_gen(child->right_sibling, table); // What we're loading into the memory

        
      // Will the retval of x cause problems?? 

      /*
      y = code_gen(child->right_sibling, table);
      add_quad(ASSN_QOP, ty, y, NULL);
      */
      t1 = NewTemp(table);
      tx = NewTemp(table);
      ty = NewTemp(table);
      x2 = NewTemp(table);
      add_quad(NULL_QOP, NULL, NULL, NULL);
//      x2 = code_gen(child, table);      
      add_quad(INDEX_QOP, x2, x, t0);
      add_quad(ASSN_QOP, tx, x2, NULL);
      add_quad(NULL_QOP, NULL, NULL, NULL);

      t2 = create_symnode("1", TEMP_LT, NULL, -1);
      t2->hasVal = 1;
      t2->val = 1;
      add_quad(ASSN_QOP, ty, t2, NULL);

      add_quad(SUB_QOP, t1, tx, ty);

      
      /* FINISH */

      add_quad(INDEX_ASSN_QOP, x, t0, t1); // Load the memory address of x[t0] into the mem_location of temp1

    }  
      
    break;


  case IF_N:
    // x = codeGen(leftChild)
    // L1 = label("END_IF")
    // (ifFalse, x, L1, -)
    // codeGen(rightChild)
    // (label, L1, -, -)
    x = code_gen(child, table);
    l1 = NewLabel(node->node_name, "END_IF");
    add_quad(IF_FALSE_QOP, x, l1, NULL);
    code_gen(child->right_sibling, table);
    add_quad(LABEL_QOP, l1, NULL, NULL);

    break;

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
    x = code_gen(child, table);
    l1 = NewLabel(node->node_name, "ELSE");
    l2 = NewLabel(node->node_name, "END_IF");

    add_quad(IF_FALSE_QOP, x, l1, NULL);
    code_gen(child->right_sibling, table);
    add_quad(GOTO_QOP, l2, NULL, NULL);
    add_quad(LABEL_QOP, l1, NULL, NULL);

    code_gen(child->right_sibling->right_sibling, table);
    add_quad(LABEL_QOP, l2, NULL, NULL);
    
    break;

  case PRINT_N:
    // x = codeGen(leftChild)
    // (print, x, -, -)
    x = code_gen(child, table);
    add_quad(PRINT_QOP, x, NULL, NULL);

    break;

  case READ_N:
    // NOTE: This might need more thinking through cause we're probably expecting to write to an
    // address. In this conception t0 returns the address of the string stored in memory.
    x = lookup_in_symboltable(table, child->value_string, LOCAL_VT); // Does this work for globals?
    
    // Ensure it's been assigned before
    t1 = create_symnode("0", TEMP_LT, NULL, -1);
    t1->hasVal = 1;
    t1->val = 1;

    add_quad(ASSN_QOP, x, t1, NULL);
    add_quad(READ_QOP, x, NULL, NULL);
    retval = x;
    break;

  case RETURN_N:
    // t0 = newTemp()
    // x = codeGen(leftChild)
    // (assn, t0, x, -)
    // (return, t0, -, -)
    //t0 = NewTemp(table);
    x = code_gen(child, table);
    //add_quad(ASSN_QOP, t0, x, NULL);
    add_quad(RETURN_QOP, x, NULL, NULL); 
    break;


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
    x = code_gen(child, table);
    l1 = NewLabel(node->node_name, "COND");
    l2 = NewLabel(node->node_name, "END_FOR");
    add_quad(LABEL_QOP, l1, NULL, NULL);
    y = code_gen(child->right_sibling, table);
    add_quad(IF_FALSE_QOP, y, l2, NULL);
    code_gen(child->right_sibling->right_sibling->right_sibling, table);
    code_gen(child->right_sibling->right_sibling, table);
    add_quad(GOTO_QOP, l1, NULL, NULL);
    add_quad(LABEL_QOP, l2, NULL, NULL);

    break;

  case WHILE_N:
    // L1 = newLabel("COND")
    // L2 = newLabel("END_WHILE")
    // label(L1, -, -, -)
    // x = codeGen(leftChild)
    // (ifFalse, x, L2, -)
    // codeGen(rightChild)
    // (goto, L1, -, -)
    // label(L2, -, -, -)
    l1 = NewLabel(node->node_name, "COND");
    l2 = NewLabel(node->node_name, "END_WHILE");
    add_quad(LABEL_QOP, l1, NULL, NULL);
    x = code_gen(child, table);
    add_quad(IF_FALSE_QOP, x, l2, NULL);
    code_gen(child->right_sibling, table);
    add_quad(GOTO_QOP, l1, NULL, NULL);
    add_quad(LABEL_QOP, l2, NULL, NULL);

    break;

  case DOWHILE_N:
    // L1 = newLabel("START")
    // (label, L1, -, -)
    // codeGen(rightChild)
    // x = codeGen(leftChild)
    // (ifTrue, x, L1, -) 
    l1 = NewLabel(node->node_name, "START");
    add_quad(LABEL_QOP, l1, NULL, NULL);
    code_gen(child->right_sibling, table);
    x = code_gen(child, table);
    add_quad(IF_TRUE_QOP, x, l1, NULL);
    break;

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
    y = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
    add_quad(PRE_CALL_QOP, y, NULL, NULL);
    count = 0;
    for (iterator = child; iterator != NULL; iterator = iterator->right_sibling) {
      // Are we passing an entire array as a parameter?
      t0 = NewTemp(table);
      x = code_gen(iterator, table);

      if (x->array_elem_count != 0 && iterator->left_child == NULL) {

        add_quad(ARG_ARRAY_QOP, x, NULL, NULL);
      } else {
        add_quad(ARG_QOP, x, NULL, NULL);
      }
      count++;
      /* ADD IN FUNCTIONALITY FOR NULL CHILD? */
    }

    add_quad(POST_PARAMS_QOP, y, NULL, NULL);
    y->func_arg_count = count;
    t1 = ((y->type == FUNC_INT_LT) ? NewTemp(table) : NULL);
    add_quad(POST_RET_QOP, t1, y, NULL);
    retval = t1;
    break;


  case COMPOUND_STMT_N:
    for (iterator = child; iterator != NULL; iterator = iterator->right_sibling)
      code_gen(iterator, table);
    break;

  case FUNC_N:
    // L1 = newLabel(node->value_string -- should be func name --)
    // (label, L1, -, -)
    // codeGen(child)
    if (strncmp(node->value_string, "main", 4) == 0) {
      l1 = create_symnode("L_MAIN", LABEL_LT, NULL, -1);
    } else {
      l1 = NewLabel(node->node_name, node->value_string);
    }

    add_quad(LABEL_QOP, l1, NULL, NULL);
    y = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
    assert(y);
    printf("Function's name in symtab: %s\n", y->name);
    y->func_label = l1->name; // Give the function entry in the table it's label
    printf("Function's func_label in symtab: %s\n", y->func_label);

    code_gen(child, table);
    break;


  case ID_N:
    // t0 = newTemp()
    // if is an array (check node->value_int)
      // x = codeGen(child)
      // y = lookupInSymtab()
      // (index, t0, y, x)
    // else
      // (assn, t0, lookUpInSymtab(), -)
    // return t0
    t0 = NewTemp(table);
    t1 = NewTemp(table);
    if (node->value_int != 0) {
      buff = calloc(1, sizeof(char) * 11); // Assumes int <= 10 digits
      buff[10] = '\0';
      sprintf(buff, "%d", node->value_int);

      /*
      x = create_symnode(buff, TEMP_LT, NULL, -1);
      x->hasVal = 1;
      x->val = node->value_int;
      x->mem_location = 0;
      */
      t1 = code_gen(child, table);
      y = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
      add_quad(INDEX_QOP, t0, y, t1);
      if (t1->hasVal) {
        t0->mem_location = y->mem_location + (4 * t1->val);
      } else {
        t0->mem_location = y->mem_location;
      }
      t0->absolute_address = y->absolute_address;
      t0->array_elem_count = y->array_elem_count;
      t0->vType = y->vType;
      t0->type = y->type;
      //t0->name = y->name;
      retval = t0;
    } else {
      x = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
      retval = x;
      //add_quad(ASSN_QOP, t0, x, NULL);

    }
    break;
//    retval = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
//      break;

    
  case INT_LITERAL_N :
      buff = calloc(1, sizeof(char) * 11); // Assumes int <= 10 digits
      buff[10] = '\0';
      sprintf(buff, "%d", node->value_int);
 
      retval = lookup_in_symboltable(table, buff, CONST_VT);

      if (retval == NULL) {
        retval = create_symnode(buff, TEMP_LT, NULL, -1);
        retval->hasVal = 1;
        retval->val = node->value_int;
        retval->mem_location = 0;
        buff = NULL;
      } else {
        retval->hasVal = 1;
        retval->val = node->value_int;
        retval->mem_location = 0;
      }

      break;

  case STRING_LITERAL_N :
    retval = lookup_in_symboltable(table, node->value_string, CONST_VT);
    if (retval == NULL) 
      retval = create_symnode(node->value_string, TEMP_LT, NULL, -1);
    break; 

  case DEC_ID_N :
      retval = lookup_in_symboltable(table, node->value_string, LOCAL_VT);
      if (retval) {
        if (retval->type == INT_ARRAY_LT) {
          t1 = create_symnode("0", TEMP_LT, NULL, -1);
          t1->hasVal = 1;
          t1->val = 0;

          t2 = create_symnode("1", TEMP_LT, NULL, -1);
          t2->hasVal = 1;
          t2->val = 1;
 
          add_quad(INDEX_ASSN_QOP, retval, t2, t1); // Load the memory address of x[t0] into the mem_location of temp
        }
      }
      break;

  default :
      retval = NULL;
  } // End of Case Statement



  if (changed_scope) {
    table->leaf = table->leaf->parent;
  }

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
  fprintf(fp, "(%s, ", QOP_NAME(quad->op));
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

symnode_t *NewLabel(char *nodename, char *text) {
  symnode_t *retval = NULL;
  int l1 = strlen(nodename);
  int l2 = strlen(text);

  if (strcmp(text, "main") == 0)
    return create_symnode("L_MAIN", LABEL_LT, NULL, -1);

  // string = "L_" + nodename + "_" + text + '\0'
  int len = 2 + l1 + 1 + l2 + 1;

  char *label = calloc(1, sizeof(char) * len);
  sprintf(label, "%s%s_%s", label_prefix, nodename, text);
  
   retval = create_symnode(label, LABEL_LT, NULL, -1);
  return retval;
}
