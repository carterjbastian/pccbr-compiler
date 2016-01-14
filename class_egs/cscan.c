#include <stdio.h>
#include <string.h>
#include "toktype.h"

extern char *yytext;
extern int yyleng;
extern int yylex (void);

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
    printf("%s: %d \"%s\"\n", token_name(token), yyleng, yytext);

  return 0;
}

  /* Some tokens just to check: == 3.14 "quote\"x" -6.02e23 */
