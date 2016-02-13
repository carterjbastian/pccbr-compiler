/*
 * tree_main.c    A driver to print or test the output of the pccbr parser
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
#include "ast.h"

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

int main() {

#ifdef _TESTING_
  printf("In Test Mode!\n");
  int noRoot = 0;
  int failedCount = 0;

  // For file IO
  char *infile = calloc(1, FILE_NAME_LEN);
  char *outfile = calloc(1, FILE_NAME_LEN);

  /* Loop through all the first tests (assuming Success) */
  for (int i = 0; i < TEST_COUNT - 1; i++) {
    // Open the input file
    sprintf(infile, "%s%s%s", TEST_PATH, test_names[i], TEST_EXT);
    FILE *f_in = fopen(infile, "r");
    yyin = f_in;
    printf("PARSER TESTING FILE: %s\n", infile);

    // Parse it
    noRoot = yyparse();

    // Open output file
    sprintf(outfile, "%s%s%s", OUT_PATH, test_names[i], OUT_EXT);
    FILE *f_out = fopen(outfile, "w");

    if (parseError && (!noRoot)) {
      fprintf(f_out, 
          "WARNING: There were %d parse errors.\nParse tree may be ill-formed.\n", 
          parseError);
      failedCount++;
      fprintf(stderr, "Parser failed to parse file %s\n", infile);
    }

    if (! noRoot) {
      print_ast(f_out, root, 0);
    } else {
      failedCount++;
      fprintf(f_out, "Parsing reached a fatal error. No AST was constructed\n");
      printf("Parser reached fatal error in %s\n", infile);
    }

    // Reset stuff
    noRoot = 0;
    root = NULL;
    fclose(f_in);
    fclose(f_out);
    memset((void *)infile, 0, FILE_NAME_LEN);
    memset((void *)outfile, 0, FILE_NAME_LEN);
    parseError = 0;
  }

  /* Complete the last test (error report and recovery) */
  sprintf(infile, "%s%s%s", TEST_PATH, test_names[TEST_COUNT - 1], TEST_EXT);
  FILE *f_in = fopen(infile, "r");
  yyin = f_in;
  printf("PARSER TESTING FILE: %s\n", infile);

  // Parse it
  sprintf(outfile, "%s%s%s", OUT_PATH, test_names[TEST_COUNT - 1], OUT_EXT);
  FILE *f_out = fopen(outfile, "w");
  error_out = f_out;  
  noRoot = yyparse();

  if (parseError != REPORTED_ERROR_COUNT && (!noRoot)) {
    fprintf(f_out, "Parser failed to catch correct number of errors\n");
    failedCount++;
    fprintf(stderr, "Parser failed to parse file %s\n", infile);
  }
 
  if (!noRoot) {
    print_ast(f_out, root, 0);
  } else {
    failedCount++;
    fprintf(f_out, "Parsing reached a fatal error. No AST was constructed\n");
    printf("Parser reached fatal error in %s\n", infile);
  }

  printf("Parser testing completed: %d tests failed\n", failedCount);

  // Garbage collection
  fclose(f_in);
  fclose(f_out);
  free(infile);
  free(outfile);
  
  return 0;

#else // _TEST_MODE_
  int noRoot = 0;		/* 0 means we will have a root */

  /*   yydebug = 1; */ // Good for debugging, unnecessary for now

  /* Build the tree */
  error_out = stderr;
  noRoot = yyparse();

  if (parseError && (!noRoot))
    fprintf(stderr, "WARNING: There were %d parse errors.\nParse tree may be ill-formed.\n", parseError);

  if (! noRoot)
    print_ast(stdout, root, 0);
  else
    fprintf(stderr, "Parsing reached fatal error. No AST was constructed\n");

  return 0;

#endif // _TEST_MODE_
}
