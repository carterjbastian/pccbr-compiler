#include <stdio.h>
#include <string.h>
#include "toktype.h"

extern FILE *yyin;
extern char *yytext;
extern int yyleng;
extern int yylex (void);
extern int yylineno;

static char *test_path = "testfiles/";
static char *test_files[] = { "token_test.c" };
static int test_tokens[] = { 258, 258, 258 };

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
  char *file_path;
  FILE *input;
 
  if (argc > 1 && strcmp(argv[1], "test") == 0) {
    input = fopen("testfiles/test1.c", "r");
    yyin = input;
    while ((token = yylex()) != EOF_T) {
        if (token != test_tokens[0]) {
            printf("token: %d\n", token);
            fprintf(stderr, "An error occured on line %d. \"%s\" is not of token type [%s].\n", yylineno, yytext, token_name(token));
            return 1;
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



