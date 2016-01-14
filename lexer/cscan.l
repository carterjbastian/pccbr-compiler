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

%x string
%x block_comment
%x inline_comment

%option yylineno
%%

{white}      /* skip whitespace */

{id}         return lexLookup(yytext);
{signedint}  return INTCONST_T;

\"           { BEGIN(string); }
<string>\n   return OTHER_T;
<string>\\\" { yymore(); }
<string>\"   {
  BEGIN(INITIAL);		/* go back to normal state */
  yytext[--yyleng] = '\0';	/* remove the trailing double-quote */
  return STRINGCONST_T;
 }
<string>.    { yymore(); }

"/*"         { BEGIN(block_comment); }
<block_comment>"*/"  { BEGIN(INITIAL); }

"//"         { BEGIN(inline_comment); }
<inline_comment>\n { BEGIN(INITIAL); }

","          return ',';
";"          return ';';
"="          return '=';
"+"          return '+';
"-"          return '-';
"*"          return '*';
"<"          return '<';
">"          return '>';
"=="         return ISEQUAL_T;
"!="         return NOTEQUAL_T;
">="         return GT_EQUAL_T;
"<="         return LT_EQUAL_T;
"&&"         return AND_T;
"||"         return OR_T;
"++"         return INCREMENT_T;
"--"         return DECREMENT_T;
"["          return '[';   
"]"          return ']'; 
"("          return '(';
")"          return ')';
"{"          return '{';
"}"          return '}'; 
"%"          return '%';
"!"          return '!';

<<EOF>>      return EOF_T;
.            return OTHER_T;

%%

int lexLookup(char *s) {
  typedef struct combo {
    char *keyword;
    int token;
  } combo;

  const int KWCOUNT = 10;

  static combo keywords[] =
    { { "int", INT_T },
      { "while", WHILE_T },
      { "for", FOR_T },
      { "if", IF_T },
      { "else", ELSE_T },
      { "return", RETURN_T },
      { "void", VOID_T },
      { "read", READ_T },
      { "print", PRINT_T },
      { NULL, ID_T }
    };

  combo *kw;
  
  keywords[KWCOUNT-1].keyword = s;

  for (kw = keywords; strcmp(kw->keyword, s) != 0; kw++)
    ;

  return kw->token;
}
