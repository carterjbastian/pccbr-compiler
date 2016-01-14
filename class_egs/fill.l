%{
#include <stdlib.h>

  int fill_column = -1;
  int next_output_column = 1;

  void get_fill_column();
  void write_string(char *string, int len);
%}
%%
[^ \t\n]+	write_string(yytext, yyleng);
[ \t\n]+	;
%%
void get_fill_column() {
  char *fill_str = getenv("FILLCOL");
  if (fill_str == NULL)
    fill_column = 20;
  else
    fill_column = atoi(fill_str);
  if (fill_column < 1) {
    fprintf(stderr, "fill: %s fill column\n",
	    fill_column == 0 ? "zero" : "negative");
    exit(-1);
  }
}

void write_string(char *string, int len) {
  if (fill_column < 1)
    get_fill_column();

  if (next_output_column > 1 && next_output_column + len > fill_column) {
    printf("\n");
    next_output_column = 1;
  }

  printf("%s%s", next_output_column == 1 ? "" : " ", string);
  next_output_column += len + (next_output_column > 1);
}

int yywrap(void) {
  if (next_output_column > 1)
    printf("\n");
  exit(0);
}
