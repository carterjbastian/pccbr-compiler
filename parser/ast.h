/* 
 * ast.h    A header defining the types of ast_nodes as well as the ast_node itself
 *
 * Authors:
 *  Originally written by THC for CS 57 and massaged by SWS.
 *  Extended for use in the pccbr-compiler project by Carter J. Bastian
 *    and Quinn Stearns in 2016.
 * 
 * This File Contains:
 *  - An enum and a struct for node types in an abstract syntax tree
 *  - Declarations of ast-related functions (implemented in ast.c)
 *
 */
//#include "symtab.h"

#ifndef AST_H_
#define AST_H_

#include <stdio.h>

/* 
 * The full enumeration of types of nodes in an Abstract Syntax Tree.
 */


/* The full enumeration of types of lbles in the C subset */
typedef enum { INT_LT,
               INT_ARRAY_LT,
               FUNC_INT_LT,
               FUNC_VOID_LT,
               ERROR_LT} var_lookup_type;

typedef struct {
  int val;
  char *name;
} type_name_pair;


typedef enum { ROOT_N,
	       OP_ASSIGN_N, OP_PLUS_N, OP_MINUS_N, OP_NEG_N, OP_TIMES_N, OP_DIVIDE_N,
               OP_MOD_N, OP_LT_N, OP_LTE_N, OP_GT_N, OP_GTE_N, OP_EQUALS_N, 
               OP_NOTEQUALS_N, OP_AND_N, OP_OR_N,
               OP_COMPLEMENT_N, OP_UNARYM_N, OP_INCREMENT_N, OP_DECREMENT_N,
               FUNC_CALL_N,
               IF_N, IF_ELSE_N, PRINT_N, READ_N, RETURN_N, FOR_N, 
               WHILE_N, DOWHILE_N, COMPOUND_STMT_N,  
               DECLARATION_LIST_N, PARAM_N, ARRAY_PARAM_N, FUNC_N, NULL_N,
	       ID_N, DEC_ID_N, STRING_LITERAL_N, INT_LITERAL_N, ERROR_N} ast_node_type;

/* A struct used for ast_node_type -> string value conversion table */
typedef struct {
        int val;
        char *name;
} val_name_pair;


/*
 * A table of ast_node_type -> string representation pairs
 * NOTE: These must be in the same order as the enumerated values in the
 *  ast_node_type.
 */
static val_name_pair token_table[] = {
  { ROOT_N, "ROOT" },
  { OP_ASSIGN_N, "=" },
  { OP_PLUS_N, "+" },
  { OP_MINUS_N, "-" },
  { OP_NEG_N, "-" },
  { OP_TIMES_N, "*" },
  { OP_DIVIDE_N, "/" },
  { OP_MOD_N, "%" },
  { OP_LT_N, "<" },
  { OP_LTE_N, "<=" },
  { OP_GT_N, ">" },
  { OP_GTE_N, ">=" },
  { OP_EQUALS_N, "==" },
  { OP_NOTEQUALS_N, "!=" },
  { OP_AND_N, "&&" },
  { OP_OR_N, "||" },
  { OP_COMPLEMENT_N, "!" },
  { OP_UNARYM_N, "U-" },
  { OP_INCREMENT_N, "++" },
  { OP_DECREMENT_N, "--" },
  { FUNC_CALL_N, "FUNC_CALL" },
  { IF_N, "IF_STMT" },
  { IF_ELSE_N, "IF_ELSE_STMT" },
  { PRINT_N, "PRINT_STMT" },
  { READ_N, "READ_STMT" },
  { RETURN_N, "RETURN_STMT" },
  { FOR_N, "FOR_STMT" },
  { WHILE_N, "WHILE_STMT" },
  { DOWHILE_N, "DOWHILE_STMT" },
  { COMPOUND_STMT_N, "COMPOUND"},
  { DECLARATION_LIST_N, "DECLARATION_LIST" },
  { PARAM_N, "PARAMETER" },
  { ARRAY_PARAM_N, "ARRAY_PARAMETER" },
  { FUNC_N, "FUNCTION" },
  { NULL_N, "EMPTY_NODE" },
  { ID_N, "ID" },
  { DEC_ID_N, "DECLARED ID"},
  { STRING_LITERAL_N, "STRING_LITERAL" },
  { INT_LITERAL_N, "INT_LITERAL"},
  { ERROR_N, "ERROR_NODE"},
  { 0, NULL }
};

#define NODE_INDEX(X)    ( (X) - ROOT_N)
#define NODE_NAME(X)     ( token_table[ NODE_INDEX((X)) ].name)


/* Structure for nodes of the abstract syntax tree.  Uses the
   left-child/right-sibling representation, so that each node can have
   a variable number of children.  You should add or remove fields as
   appropriate. */
typedef struct ast_node_struct *ast_node;
struct ast_node_struct {
  ast_node_type node_type;
  ast_node left_child, right_sibling;

  char *value_string;		/* for IDs, errors, null nodes, etc. */
  int value_int;		/* for INT_LITERAL */

  var_lookup_type dtype;                    /* Data type for expressions and vars */
  int lineno;                   /* Line number for var declarations */  
};

/* Create a node with a given token type and return a pointer to the
   node. */
ast_node create_ast_node(ast_node_type node_type);

/* Print the contents of a subtree of an abstract syntax tree, given
   the root of the subtree and the depth of the subtree root. */
void print_ast(FILE *fp, ast_node root, int depth);

#endif // AST_H_
