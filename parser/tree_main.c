/*
 * tree_main.c
 *
 * Driver program for a parser that builds an abstract syntax tree.
 *
 * Written by THC for CS 57; massaged by SWS.  Modify this file as appropriate.
 *
 */

#include <stdio.h>
#include "ast.h"

ast_node root = NULL;

extern int yyparse();
extern int yydebug;
int parseError = 0;

int main() {
  int noRoot = 0;		/* 0 means we will have a root */

  /*   yydebug = 1; */
  noRoot = yyparse();

  printf("noroot: %d\n", noRoot);

  if (parseError)
    fprintf(stderr, "WARNING: There were parse errors.\nParse tree may be ill-formed.\n");

  print_ast(root, 0);

  return 0;
}
