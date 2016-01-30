/* symtab.h
 * Declarations for the symbol table.
 * Written by THC for CS 57.
 * You should extend the structs and functions as appropriate.
 *
 * extended and changed muchly by sean
 */

#ifndef SYMTAB_H_
#define SYMTAB_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"


/*
 * ================================================= 
 * Data Structures for Types in Lookup Tables
 * =================================================
 */

/* The full enumeration of types of lbles in the C subset */
typedef enum { INT_LT, 
               FUNC_INT_LT,
               FUNC_VOID_LT,
               ERROR_LT} var_lookup_type;

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
  { FUNC_INT_LT, "FUNCTION_RETURNING_INTEGER" },
  { FUNC_VOID_LT, "FUNCTION_RETURNING_VOID" },
  { ERROR_LT, "VARIABLE_WITH_INVALID_DECLARATION" },
  { 0, NULL }
};

#define LT_INDEX(X)     ( (X) - INT_LT )
#define LT_NAME(X)      ( lt_table[ LT_INDEX((X)) ].name )


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
  /* 
   * If the node is a function, point to it's generic function
   * table
   */
  symhashtable *generic_func_table; 
  /* Other attributes go here. */
} symnode_t;


/* Hash table for a given scope in a symbol table. */

typedef struct symhashtable {
  char *name;
  int size;			/* size of hash table */
  symnode_t **table;		/* hash table */
  int level;			/* level of scope, 0 is outermost */
  int sibno;                    /* 0 is leftmost */
  struct symhashtable *parent, *child, *rightsib;
  
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

/* Set the name in this node. */
void set_node_name(symnode_t *node, char *name);

/* Does the identifier in this node equal name? */
int name_is_equal(symnode_t *node, char *name);

/*
 * =================================================
 * symhashtable function prototypes
 * =================================================
 */

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
symnode_t *insert_into_symboltable(symboltable_t *symtab, char *name);

/* Lookup an entry in a symbol table.  If found return a pointer to it.
   Otherwise, return NULL */
symnode_t *lookup_in_symboltable(symboltable_t *symtab, char *name);


/* Enter a new scope. */
void enter_scope(symboltable_t *symtab, int type, ast_node node);

/* Leave a scope. */
void leave_scope(symboltable_t *symtab);

/* print the table in a nice way */
void print_symtab(symboltable_t *symtab);

#endif
