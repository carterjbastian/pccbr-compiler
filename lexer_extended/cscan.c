#include <stdio.h>
#include <string.h>
#include "toktype.h"

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

int main() {
  int token;

  while ((token = yylex()) != EOF_T)
    printf("[%d]: %s \"%s\"\n", yylineno, token_name(token), yytext);

  return 0;
}
