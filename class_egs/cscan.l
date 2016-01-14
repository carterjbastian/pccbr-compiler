%{
#include <string.h>
#include "toktype.h"
int lexLookup(char *);
%}

white [ \t\n]+
letter [A-Za-z]
digit [0-9]
id ({letter}|_)({letter}|{digit}|_)*
integer {digit}+
signedint [-+]?{integer}
fixedfloat {signedint}"."{integer}?|[-+]?"."{integer}
expfloat {fixedfloat}[Ee]{signedint}|{signedint}[Ee]{signedint}

%x string

%%

{white}      /* skip whitespace */

{id}         return lexLookup(yytext);
{signedint}  return INTCONST_T;
{fixedfloat} |
{expfloat}   return FLOATCONST_T;

\"           { BEGIN(string); }
<string>\n   return OTHER_T;
<string>\\\" { yymore(); }
<string>\"   {
  BEGIN(INITIAL);		/* go back to normal state */
  yytext[--yyleng] = '\0';	/* remove the trailing double-quote */
  return STRINGCONST_T;
 }
<string>.    { yymore(); }

";"          return SEMICOLON_T;
"="          return ASSIGN_T;
"+"          return PLUS_T;
"-"          return MINUS_T;
"*"          return ASTERISK_T;
"#"          return SHARP_T;
"<"          return LESS_T;
">"          return GREATER_T;
"=="         return ISEQUAL_T;
"!="         return NOTEQUAL_T;
"<<"         return SHL_T;
"\."         return DOT_T;
"->"         return ARROW_T;
"["          return '[';   /* a valid but odd alternate: */
"]"          return ']';   /* [][(){}]     return yytext[0]; */
"("          return '(';
")"          return ')';
"{"          return '{';
"}"          return '}'; 

<<EOF>>      return EOF_T;
.            return OTHER_T;

%%

int lexLookup(char *s) {
  struct combo {
    char *keyword;
    int token;
  };
  typedef struct combo combo;

  const int KWCOUNT = 7;

  static combo keywords[] =
    { { "include", INCLUDE_T },
      { "extern", EXTERN_T },
      { "int", INT_T },
      { "char", CHAR_T },
      { "while", WHILE_T },
      { "for", FOR_T },
      { NULL, ID_T }
    };

  combo *kw;
  
  keywords[KWCOUNT-1].keyword = s;

  for (kw = keywords; strcmp(kw->keyword, s) != 0; kw++)
    ;

  return kw->token;
}
