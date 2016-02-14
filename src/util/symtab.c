/* symtab.c   a file implementing functions to work with symbol tables
 *
 * Authors:
 *  Originally written by THC for CS57 and modified by SWS
 *  Extended for use in the pccbr compiler by Carter J. Bastian and
 *    Quinn Stearns in 2016
 *
 * This file Contains:
 *  - functions to implement groups of symbol information (symnode_t)
 *  - functions to implement scope-specific hash tables of symbol information
 *    (symhashtable_t)
 *  - functions to link these symbol hashtables into a single, heirarchical 
 *    table that can be printed for easy viewing (symboltable_t)
 */

// System includes
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Local Includes
#include "symtab.h"

// Macros
#define NOHASHSLOT -1
#define NAME_DEPTH 85

// Global Variable for naming functionality
int levels[NAME_DEPTH] = { 0, 0 };

/*
 * Local function declarations
 */
void recursive_print_symtab(symhashtable_t *symhash, int offset);
void print_symhashtable(symhashtable_t *symhash, int offset);
void print_symnode(symnode_t *node, int offset);

/*
 * =================================================
 * Functions for symnodes.
 * =================================================
 */
symnode_t *create_symnode(char *name, var_lookup_type type, symhashtable_t *parent, int lineno) {
  symnode_t *node = (symnode_t *) calloc(1, sizeof(symnode_t));
  assert(node);
  node->name = name;
  node->type = type;
  node->parent = parent;
  node->lineno = lineno;

  return node;
}

/*
 * Returns 0 if the name is equal to the name of the symnode
 * Returns nonzero otherwise.
 * Assumes both strings involved are null-terminated!!
 */
int name_is_equal(symnode_t *node, char *name) {
  return (strcmp(node->name, name) == 0);
}
/*
 * =================================================
 * Functions for symhashtables.
 * ================================================= 
 */

/* Create an empty symhashtable and return a pointer to it.  The
   parameter entries gives the initial size of the table. */
symhashtable_t *create_symhashtable(int entries) {
  symhashtable_t *hashtable = calloc(1, sizeof(symhashtable_t));
  assert(hashtable);
  hashtable->size = entries;
  hashtable->table = calloc(entries, sizeof(symnode_t *));

  assert(hashtable->table);


  /* anything else ? */
  
  return hashtable;
}



/* Peter Weinberger's hash function, from Aho, Sethi, & Ullman
   p. 436. */
static int hashPJW(char *s, int size) {
  unsigned h = 0, g;
  char *p;

  for (p = s; *p != '\0'; p++) {
      h = (h << 4) + *p;
      if ((g = (h & 0xf0000000)) != 0)
	h ^= (g >> 24) ^ g;
    }

  return h % size;
}


/* Look up an entry in a symhashtable, returning either a pointer to
   the symnode for the entry or NULL.  slot is where to look; if slot
   == NOHASHSLOT, then apply the hash function to figure it out. */
symnode_t *lookup_symhashtable(symhashtable_t *hashtable, char *name,
				   int slot) {
  symnode_t *node = NULL;

  assert(hashtable);

  if (slot == NOHASHSLOT)
    slot = hashPJW(name, hashtable->size);

  for (node = hashtable->table[slot];
       node != NULL && !name_is_equal(node, name);
       node = node->next)
    ;

  return node;
}

/* Looks up a node in a symhashtable without knowing the hashslot */
symnode_t *lookup_in_symhashtable(symhashtable_t *hashtable, char *name) {
  return lookup_symhashtable(hashtable, name, NOHASHSLOT);
}


/* Insert a new entry into a symhashtable, but only if it is not
   already present. */
symnode_t *insert_into_symhashtable(symhashtable_t *hashtable, var_lookup_type type, char *name, int lineno) {

  assert(hashtable);

  int slot = hashPJW(name, hashtable->size);
  symnode_t *node = lookup_symhashtable(hashtable, name, slot);

  /* error check if node already existed! */

  if (node == NULL) {
    node = create_symnode(name, type, hashtable, lineno);
    node->next = hashtable->table[slot];
    hashtable->table[slot] = node;
  }

  return node;
}




/*
 * ================================================= 
 * Functions for symboltables.
 * =================================================
 */

static const int HASHSIZE = 211;

/* Create an empty symbol table. */
symboltable_t  *create_symboltable() {
  symboltable_t *symtab = calloc(1, sizeof(symboltable_t));
  assert(symtab);

  symhashtable_t *hashtable=create_symhashtable(HASHSIZE);
  hashtable->level = 0;
  hashtable->name = "0";

  symtab->root = hashtable;
  symtab->leaf = hashtable;
  return symtab;
}








/* 
 * instert_into_symboltable
 *
 * Insert an entry into the innermost scope of symbol table.  First 
 * make sure it's not already in that scope.  Return a pointer to the
 * entry. 
 */
symnode_t *insert_into_symboltable(symboltable_t *symtab, var_lookup_type type, char *name, int lineno) {

  assert(symtab);
  assert(symtab->leaf);
  
  symnode_t *node = lookup_symhashtable(symtab->leaf, name, NOHASHSLOT);

  /* error check!! */
  
  if (node == NULL) {
    node = insert_into_symhashtable(symtab->leaf, type, name, lineno);
    return node;
  } else {
    return NULL;
  }
   
}





/* 
 * lookup_in_symboltable
 *
 * Lookup an entry in a symbol table.  If found return a pointer to it.
 * Otherwise, return NULL 
 */
symnode_t *lookup_in_symboltable(symboltable_t  *symtab, char *name) {
  symnode_t *node;
  symhashtable_t *hashtable;


  assert(symtab);

  for (node = NULL, hashtable = symtab->leaf;
       (node == NULL) &&  (hashtable != NULL);
       hashtable = hashtable->parent) {
    node = lookup_symhashtable(hashtable, name, NOHASHSLOT);
  }

  return node;
}

/* lookup_func_def_node
 *
 * Lookup the function definition for the current node
 */
ast_node lookup_func_def_node(symboltable_t *symtab) {
  ast_node node;
  symhashtable_t *hashtable;

  assert(symtab);

  for (hashtable = symtab->leaf;
       (!hashtable->declaring_func && hashtable->parent != NULL);
       hashtable = hashtable->parent); 
  
  node = hashtable->declaring_func;
  return node;

}

/*
 * Create the name for a scope based on the current state of the levels aray
 *
 * Implements the naming protocol documented in the README
 */

static char *create_name() {
    char *name = malloc(3 * NAME_DEPTH);
    char *next_dig = malloc(5);

    sprintf(name, "%d", levels[0]);

    for (int i = 1; levels[i] != 0 && i < NAME_DEPTH ; i++) {
        sprintf(next_dig, "-%d", levels[i]);
        strncat(name, next_dig, 5);
    }

    return name;
}

/* 
 * Enter a new scope. 
 * Creates a new symhashtable at the next level and appropriate place.
 *
 */
void enter_scope(symboltable_t *symtab, ast_node definer) {
  assert(symtab);
  assert(symtab->leaf);

  symhashtable_t *curr = symtab->leaf;
  symhashtable_t *next = create_symhashtable(HASHSIZE); 
  symhashtable_t *it;

  int sibno = 0;

  levels[curr->level]++;

  // Set up the symhashtable with the correct basic values
  next->name = create_name(); // WRITE THE NAMING PROTOCOL
  next->level = curr->level + 1;
  next->declaring_func = definer;

  // Create a new symhashtable at the right place
  if (symtab->leaf->child) {
    it = symtab->leaf->child;
    sibno++;
    
    // Iterate until you find the rightmost child
    while (it->rightsib) {
      it = it->rightsib;
      sibno++;
    }
    
    // Set the new symhashtable as the rightmost child
    it->rightsib = next;
    next->sibno = sibno;
  } else {
    symtab->leaf->child = next;
    next->sibno = sibno;
  }

  // Set links for new node
  next->parent = symtab->leaf;
  next->child = NULL;
  next->rightsib = NULL;

  // Update the symboltable's leaf value
  symtab->leaf = next;

}

/* Leave a scope. */
void leave_scope(symboltable_t *symtab) {
  levels[symtab->leaf->level] = 0;
  symtab->leaf = symtab->leaf->parent;
}

/* print the table in a nice way */
void print_symtab(symboltable_t *symtab) {
  symhashtable_t *curr = symtab->root;
  int depth = 0;

  recursive_print_symtab(curr, depth);
}

/*
 * Recursive function to print a sub-table of a symbol table
 */
void recursive_print_symtab(symhashtable_t *symhash, int offset) {
  symhashtable_t *curr;
  print_symhashtable(symhash, offset);
  // Are there children to recurse on?
  if (symhash->child) {
    // Then just do it already
    curr = symhash->child;
    recursive_print_symtab(curr, offset + 1);
    // And then recurse on his bros
    while (curr->rightsib) {
      curr = curr->rightsib;
      recursive_print_symtab(curr, offset + 1);
    }
  }
}

/*
 * Function to print a single hashtable in a symbol table
 */
void print_symhashtable(symhashtable_t *symhash, int offset) {
  int max = symhash->size;
  symnode_t *curr;

  printf("| ");
  for (int i = 0; i < offset; i++)
    printf("- ");
  printf("Scope: %s\n", symhash->name);

  printf("| ");
  for (int i = 0; i < offset; i++)
    printf("- ");
  printf("Variables:\n");

  // Loop through each bucket
  for (int i = 0; i < max; i++) {

    // Is the bucket empty?
    if (symhash->table[i] != NULL) {
      // Ensure you hit all the nodes in this bucket
      for (curr = symhash->table[i]; curr != NULL; curr = curr->next) {
        print_symnode(curr, offset);
      }
    }
  }

  if (symhash->declaring_func) {
    ast_node definer = symhash->declaring_func;
    printf("| definer is %s\n", definer->value_string);
    printf("| Return type is: ");
    if (definer->value_int == 0)
      printf("void\n");
    else
      printf("int\n");
  }
  printf("\n");
}

/*
 * Function to print a single node of symbol information in a nice way
 * with semantically meaningful indenting
 */
void print_symnode(symnode_t *node, int offset) {
  printf("| ");
  for (int i = 0; i < offset; i++)
    printf("- ");

  printf("%s\tType: %s\tLine: %d\n", node->name, LT_NAME(node->type), node->lineno);
}

/* 
 * Function: print_checked_ast
 *  @root   - the root of the current subtree of an AST
 *  @depth  - the current overall depth into the AST
 *
 * Recursively prints the contents of a subtree of an abstract syntax tree, 
 * when provided with the root of the subtree and the 
 * depth of the subtree root. 
 */
void print_checked_ast(FILE *fp, ast_node root, int depth) {
  /* Print two spaces for every level of depth. */
  int i;

  assert(root);
  
  /* Syntactic sugar */
  fprintf(fp, "| ");
  for (i = 1; i < depth; i++)
    fprintf(fp, "- ");

  /* Print the node type. */
  fprintf(fp, "%s ", NODE_NAME(root->node_type));

  /* Print attributes specific to node types. */
  switch (root->node_type) {
  case ID_N:      /* print the id */
    assert(root->value_string);
    fprintf(fp, " (%s declared on line %d)", root->value_string, root->lineno);
    fprintf(fp, " (type = %s)", LT_NAME(root->dtype));

    if (root->value_int != 0) { 
      if (root->value_int > 0)
        fprintf(fp, " (array of length %d)", root->value_int);
      else
        fprintf(fp, " (array indexed to left-child expression)");
    }


    break;

  case DEC_ID_N:
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);

    if (root->value_int != 0) { 
      if (root->value_int > 0)
        fprintf(fp, " (array of length %d)", root->value_int);
      else
        fprintf(fp, " (array indexed to left-child expression)");
    }    
    break;

  case INT_LITERAL_N:   /* print the int literal */
    fprintf(fp, "%d", root->value_int);
    break;

  case STRING_LITERAL_N:        /* print the value of the string literal */
    assert(root->value_string);
    fprintf(fp, "\"%s\"", root->value_string);
    break;

  case PARAM_N:                 /* Print the name of the param */
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);
    break;

  case ARRAY_PARAM_N:           /* Print the name of the array parameter */
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);
    break;

  case FUNC_CALL_N:             /* Print the name of the function being called */
    assert(root->value_string);
    fprintf(fp, "%s", root->value_string);
    break;

  case NULL_N:                  /* Print the syntactic purpose of the null node */
    fprintf(fp, "\t%s", root->value_string);
    break;

  case FUNC_N:                  /* Print the name of the function */
    fprintf(fp, "%s", root->value_string);
    break;

  case ERROR_N:                 /* Print the type of error represented */
    assert(root->value_string);
    fprintf(fp, "\t%s", root->value_string);
    break;

  case OP_ASSIGN_N ... OP_DECREMENT_N :
    fprintf(fp, " (dtype = %s)", LT_NAME(root->dtype));
    break;

  default:
    break;
  }

  fprintf(fp, "\n");

  /* Recurse on each child of the subtree root, with a depth one
     greater than the root's depth. */
  ast_node child;
  for (child = root->left_child; child != NULL; child = child->right_sibling)
    print_checked_ast(fp, child, depth + 1);

}
