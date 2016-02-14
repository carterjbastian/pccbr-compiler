/*
 * symbols_main.c    A driver for the pccbr symbol table generator
 *
 * Authors:
 *  Originally written by THC for CS 57 and massaged by SWS.
 *  Extended for use in the pccbr-compiler project by Carter J. Bastian
 *    and Quinn Stearns in 2016.
 *
 * This File Contains:
 *  - Driver program for a parser that builds an abstract syntax tree
 *  - A testing suite for the parser to demonstrate that it works
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util/ast.h"
#include "../util/symtab.h"
#include "../symtab/buildtab.h"
#include "tree_check.h"

/* Only include info needed for autoTesting if in Test Mode */
#ifdef _TESTING_
#include "testfiles/autoTest.h"
#endif

ast_node root = NULL;

extern int yyparse();
extern int yydebug;
extern FILE *yyin;

// Used in the parser (parser.y) for error recovery and reporting.
int parseError = 0;
FILE *error_out;  // Global conditionally used in testing mode only

/*
 * Build the AST and create a Symbol Table for it
 */
int main() {
  int noRoot = 0;		/* 0 means we will have a root */
  symboltable_t *symtab;

  /* Build the tree */
  error_out = stderr;
  noRoot = yyparse();

  if (parseError && (!noRoot))
    fprintf(stderr, "WARNING: There were %d parse errors.\nParse tree may be ill-formed.\n", parseError);

  if (noRoot)
    fprintf(stderr, "Parsing reached fatal error. No AST was constructed\n");

  /* Set up the symbol tree */
  symtab = create_symboltable();
  symtab = build_symboltable(symtab, root, root);
  printf("Symtable created...\n");
  print_symtab(symtab);
  print_ast(stdout, root, 0);

  int retval = typecheck_ast(symtab, root);
  print_checked_ast(stdout, root, 0);

  return 0;

}
