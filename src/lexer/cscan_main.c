/*
 * File: cscan.c
 *  A file containing the main function for scanning c-code via flex
 *
 * Authors: Carter J. Bastian, Quinn Stearns
 *    CSOC57, 15W
 *
 * Special Considerations:
 *  - Based heavily on a similar file written by Thomas H. Cormen and Sean Smith
 *
 */

/* System Includes */
#include <stdio.h>
#include <string.h>

/* Local Includes */
#include "toktype.h"

#ifdef TEST_MODE_
#include "tests/lex_tests.h"
#endif 

/* Public Variable Declarations */
extern FILE *yyin;
extern char *yytext;
extern int yyleng;
extern int yylex (void);
extern int yylineno;

char *token_name(int token) {
  char tiny_buf[2];

  if (token > 256) // non-single-char
    return (tokens[token - FIRST_BIG_TOKEN]);

  tiny_buf[1] = 0x00;
  tiny_buf[0] = token;
  return strdup(tiny_buf);
}

/* The main function to do the scanning via flex */
int main() {

#ifdef TEST_MODE_
  int token;
  FILE *input;
  int errors = 0;
  
  printf("Testing the lexer\n");
  for (int i = 0; i < TEST_COUNT; i++) {
      // Loop through each test file 
      printf("LEXER TESTING FILE: %s\n", test_files[i]);
      input = fopen(test_files[i], "r");
      yyin = input;
      int count = 0;

      // Scan each token in the test files
      while ((token = yylex()) != EOF_T) {
        // Check for errors (against the known correct results)
        if (token != test_comparison[i][count]) {
          // Report the discrepency
          fprintf(stderr, 
              "An error occured on token %d. \"%s\" is not of token type [%s].\n", 
              count, 
              yytext, 
              token_name(test_comparison[i][count]));
          fprintf(stderr, 
              "Text %s returned token type %s\n",
              yytext,
              token_name(token));
          errors++;
        }
          
        count += 1;
      }
  } 
  printf("Lexer testing completed: %d tests failed\n", errors);

#else // Not in testing mode – do normal scanning
  int token;
  while ((token = yylex()) != EOF_T)
    printf("[%d]: %s \"%s\"\n", yylineno, token_name(token), yytext);
  
#endif // TEST_MODE_

  return 0;
}
