/* symtab.c
 * Functions for the symbol table.
 * Written by THC for CS 57.
 *
 * extended and changed by sws
 *
 * You should extend the functions as appropriate.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symtab.h"

#define NOHASHSLOT -1

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
symnode_t *create_symnode(char *name, var_lookup_type type, symhashtable_t *parent) {
  symnode_t *node = (symnode_t *) calloc(1, sizeof(symnode_t));
  assert(node);
  node->name = name;
  node->type = type;
  node->parent = parent;

  return node;
}

/*
 * Returns 0 if the name is equal to the name of the symnode
 * Returns nonzero otherwise.
 * Assumes both strings involved are null-terminated!!
 */
int name_is_equal(symnode_t *node, char *name) {
  return strcmp(node->name, name);
}
/*
 * =================================================
 * Functions for symhashtables.
 * ================================================= 
 */

/* Create an empty symhashtable and return a pointer to it.  The
   parameter entries gives the initial size of the table. */
symhashtable_t *create_symhashtable(int entries) {
  symhashtable_t *hashtable = malloc(sizeof(symhashtable_t));
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
  symnode_t *node;

  assert(hashtable);

  if (slot == NOHASHSLOT)
    slot = hashPJW(name, hashtable->size);

  for (node = hashtable->table[slot];
       node != NULL && !name_is_equal(node, name);
       node = node->next)
    ;

  return node;
}






/* Insert a new entry into a symhashtable, but only if it is not
   already present. */
symnode_t *insert_into_symhashtable(symhashtable_t *hashtable, var_lookup_type type, char *name) {

  assert(hashtable);

  int slot = hashPJW(name, hashtable->size);
  symnode_t *node = lookup_symhashtable(hashtable, name, slot);

  /* error check if node already existed! */

  if (node == NULL) {
    node = create_symnode(name, type, hashtable);
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
  symboltable_t *symtab = malloc(sizeof(symboltable_t));
  assert(symtab);

  symhashtable_t *hashtable=create_symhashtable(HASHSIZE);
  hashtable->level = 0;
  hashtable->name = "0";

  symtab->root = hashtable;
  symtab->leaf = hashtable;
  return symtab;
}








/* Insert an entry into the innermost scope of symbol table.  First
   make sure it's not already in that scope.  Return a pointer to the
   entry. */
symnode_t *insert_into_symboltable(symboltable_t *symtab, var_lookup_type type, char *name) {

  assert(symtab);
  assert(symtab->leaf);
  
  symnode_t *node = lookup_symhashtable(symtab->leaf, name, NOHASHSLOT);

  /* error check!! */
  
  if (node == NULL) {
    node = insert_into_symhashtable(symtab->leaf, type, name);
    return node;
  } else {
    return NULL;
  }
   
}





/* Lookup an entry in a symbol table.  If found return a pointer to it.
   Otherwise, return NULL */
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


/* 
 * Enter a new scope. 
 * Creates a new symhashtable at the next level and appropriate place.
 *
 */
void enter_scope(symboltable_t *symtab, int type, ast_node node) {
  assert(symtab);
  assert(symtab->leaf);

  symhashtable_t *curr = symtab->leaf;
  symhashtable_t *next = create_symhashtable(HASHSIZE); 
  symhashtable_t *it;

  int sibno = 0;

  // Set up the symhashtable with the correct basic values
  next->name = "Fred"; // WRITE THE NAMING PROTOCOL
  next->level = curr->level + 1;

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
  symtab->leaf = symtab->leaf->parent;
}

/* print the table in a nice way */
void print_symtab(symboltable_t *symtab) {
  symhashtable_t *curr = symtab->root;
  int depth = 0;

  recursive_print_symtab(curr, depth);
}

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

void print_symhashtable(symhashtable_t *symhash, int offset) {
  int max = symhash->size;
  symnode_t *curr;

  // Loop through each bucket
  for (int i = 0; i < max; i++) {
    // Is the bucket empty?
    if (symhash->table[i]) {
      curr = symhash->table[i];
      print_symnode(curr, offset);

      // Ensure you hit all the nodes in this bucket
      while (curr->next) {
        curr = curr->next;
        print_symnode(curr, offset);
      }
    }
  }
}

void print_symnode(symnode_t *node, int offset) {
  for (int i = 0; i < offset; i++)
    printf("- ");

  printf("%s\tType: %s", node->name, LT_NAME(node->type));
}