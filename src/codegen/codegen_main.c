#include "../util/ast.h"
#include "../util/symtab.h"
#include "../symtab/buildtab.h"
#include "../quadgen/quadgen.h"
#include "../symcheck/tree_check.h"
#include "codegen.h"

#include <stdio.h>

ast_node root = NULL;
quad_t list;

extern int yyparse();
extern int yydebug;
extern FILE *yyin;

// Used in the parser (parser.y) for error recovery and reporting.
int parseError = 0;
FILE *error_out;  // Global conditionally used in testing mode only

int main() {
  FILE *ys = fopen("./sample.ys", "w");
  int noRoot = 0;		/* 0 means we will have a root */
  symboltable_t *symtab;
  list = NULL;

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
  typecheck_ast(symtab, root);
  int retval = typecheck_ast(symtab, root);
  print_checked_ast(stdout, root, 0);
  if (retval != 0) {
    fprintf(stderr, "There were %d errors encountered in the parse tree. Aborting.\n", retval);
    return 1;
  } 
  print_ast(stdout, root, 0);
  print_symtab(symtab);
  code_gen(root, symtab);
  print_symtab(symtab);

  print_quad_list(stdout, list);
  generate_assembly(ys, list, symtab);
  return 0;
}
