#include "quadgen.h"

ast_node root = NULL;

extern int yyparse();
extern int yydebug;
extern FILE *yyin;

// Used in the parser (parser.y) for error recovery and reporting.
int parseError = 0;
FILE *error_out;  // Global conditionally used in testing mode only

int main() {
  // Build syntax tree
  ast_node root = create_ast_node(NULL_N);
  // TODO: Build symbol table
  symboltable_t *table = NULL;
  // Ignore symbol table checking for now?
  // codeGen(root)
  code_gen(root, table);
}
