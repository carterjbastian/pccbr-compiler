#include "../util/ast.h"
#include "../util/symtab.h"
#include "../symtab/buildtab.h"
#include "quadgen.h"

ast_node root = NULL;

extern int yyparse();
extern int yydebug;
extern FILE *yyin;

// Used in the parser (parser.y) for error recovery and reporting.
int parseError = 0;
FILE *error_out;  // Global conditionally used in testing mode only

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
//  printf("Symtable created...\n");
//  print_symtab(symtab);
//  print_ast(stdout, root, 0);

  code_gen(root, symtab);

  return 0;
}