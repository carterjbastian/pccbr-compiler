#include <stdio.h>
#include <string.h>
#include "toktype.h"

extern FILE *yyin;
extern char *yytext;
extern int yyleng;
extern int yylex (void);
extern int yylineno;

static char test_path[] = "testfiles/";

static int test_file_ct = 1;
static char *test_files[] = { "token_test.c" };
static int test_tokens[] = { ID_T, ID_T, ID_T };

char *token_name(int token) {
  char tiny_buf[2];

  if (token > 256) // non-single-char
    return (tokens[token - FIRST_BIG_TOKEN]);

  tiny_buf[1] = 0x00;
  tiny_buf[0] = token;
  return strdup(tiny_buf);
}

int main(int argc, char *argv[]) {
  int token;
  char file_path[80];
  FILE *input;
 
  if (argc > 1 && strcmp(argv[1], "test") == 0) {
    for (int i = 0; i < test_file_ct; i++) {
        strncpy(file_path, test_path, 40);
        strncat(file_path, test_files[i], 40);
        input = fopen(file_path, "r");
        yyin = input;
        int count = 0;
        while ((token = yylex()) != EOF_T) {
            if (token != test_tokens[0]) {
                fprintf(stderr, "An error occured on line %d. \"%s\" is not of token type [%s].\n", yylineno, yytext, token_name(token));
                return 1;
            }

            count += 1;
        }
    }
    
    printf("Congratulations! All tests ran successfully!\n");
  }

  else {
    while ((token = yylex()) != EOF_T)
      printf("[%d]: %s \"%s\"\n", yylineno, token_name(token), yytext);
  }

  return 0;
}



