/* 
 * symtab.h   A header defining the types of symbol nodes as well as symbol table
 *            data structures.
 * Authors:
 *  Originally written by THC for CS 57 and massaged by SWS
 *  Extended for use in the pccbr-compiler project by Carter J. Bastian
 *    and Quinn Stearns in 2016
 *
 * This File Contains:
 *  - An enum and a struct for types of symbols in a symbol table
 *  - Definitions of symbol-table related data structures
 *  - Prototypes of functions needed to implement the symbol table structures
 *
 */

#ifndef SYMTAB_H_
#define SYMTAB_H_ 

// System Includes
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Local Includes
#include "ast.h"
#include "../share/types.h"

/*
 * ================================================= 
 * Data Structures for Types in Lookup Tables
 * =================================================
 */

/* NOTE: var_lookup_type */

/* A struct used for ast_node_type -> string value conversion table */
typedef struct {
  int val;
  char *name;
} type_name_pair;

/*
 * A Table of var_lookup_type -> string representation pairs.
 * NOTE: These must be in the same order as the enumerated values in
 * the var_lookup_type
 */
static type_name_pair lt_table[] = {
  { INT_LT, "INTEGER" },
  { INT_ARRAY_LT, "ARRAY_OF_INTEGERS" },
  { FUNC_INT_LT, "FUNCTION_RETURNING_INTEGER" },
  { FUNC_VOID_LT, "FUNCTION_RETURNING_VOID" },
  { STR_LT, "CHAR_STRING" },
  { VOID_LT, "VOID_VALUE" },
  { ERROR_LT, "VARIABLE_WITH_INVALID_DECLARATION" },
  { 0, NULL }
};

#define LT_INDEX(X)     ( (X) - INT_LT )
#define LT_NAME(X)      ( lt_table[ LT_INDEX((X)) ].name )

typedef enum { LOCAL_VT, TEMP_VT, CONST_VT } var_type;

static type_name_pair vt_table[] = {
  { LOCAL_VT, "LOCAL_VARIABLE" },
  { TEMP_VT, "TEMP_VARIABLE" },
  { CONST_VT, "CONSTANT_VARIABLE" },
  { 0, NULL }
};

#define VT_INDEX(X)     ( (X) - LOCAL_VT )
#define VT_NAME(X)      ( vt_table[ VT_INDEX((X)) ].name )


/*
 * =================================================
 * Data Types for symnode, symhashtable, and symboltable 
 * =================================================
 */
typedef struct symnode {
  char *name;	                    /* name in this symnode */
  struct symnode  *next;	    /* next symnode in list */
  struct symhashtable *parent;
  var_lookup_type type;             /* The type of the symbol */
  int lineno;                       /* Where in the file the symbol was declared */
  var_type vType;                   /* The type of the variable (temp, const, local) */
} symnode_t;

/* Hash table for a given scope in a symbol table. */

typedef struct symhashtable {
  char *name;
  int size;			/* size of hash table */
  symnode_t **table;		/* hash table */
  int level;			/* level of scope, 0 is outermost */
  int sibno;                    /* 0 is leftmost */
  struct symhashtable *parent, *child, *rightsib;
  struct symhashtable *next_child;
  ast_node declaring_func;
  
} symhashtable_t;

/* Symbol table for all levels of scope. */
typedef struct {
  symhashtable_t *root, *leaf;
} symboltable_t;

/* 
 * =================================================
 * symnode function prototypes
 * =================================================
 */

/* Create and return an empty symnode */
symnode_t *create_symnode(char * name, var_lookup_type type, symhashtable_t *parent, int lineno);

/* Does the identifier in this node equal name? */
int name_is_equal(symnode_t *node, char *name);

/*
 * =================================================
 * symboltable function prototypes
 * =================================================
 */

/* Create an empty symbol table. */
symboltable_t *create_symboltable();

/* Insert an entry into the innermost scope of symbol table.  First
   make sure it's not already in that scope.  Return a pointer to the
   entry. */
symnode_t *insert_into_symboltable(symboltable_t *symtab, var_lookup_type type, char *name, int lineno);

symnode_t *insert_constant(symboltable_t *symtab, var_lookup_type type, char *name, int lineno);

/* Lookup an entry in a symbol table.  If found return a pointer to it.
   Otherwise, return NULL */
symnode_t *lookup_in_symboltable(symboltable_t *symtab, char *name);

/* Lookup an entry in a symbol table. If found return symbol_t*.
   Otherwise, return NULL */
symnode_t *lookup_in_symhashtable(symhashtable_t *hashtable, char *name); 

/* Look up calling function declaration node for current scope 
   Return the calling node; If no caller return NULL */
ast_node lookup_func_def_node(symboltable_t *symtab);

/* Enter a new scope. */
//void enter_scope(symboltable_t *symtab, int type, ast_node node);
void enter_scope(symboltable_t *symtab, ast_node caller);

/* Leave a scope. */
void leave_scope(symboltable_t *symtab);

/* Change to next child scope. */
void change_scope(symboltable_t *symtab);

/* print the table in a nice way */
void print_symtab(symboltable_t *symtab);
#endif
