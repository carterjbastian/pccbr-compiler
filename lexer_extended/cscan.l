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
char_literal '.'

%x string
%x block_comment
%x inline_comment

%option yylineno
%%

{white}      /* skip whitespace */

{id}         return lexLookup(yytext);
{signedint}  return INTCONST_T;

  /* Extra credit for character literals */
{char_literal} return CHAR_LITERAL_T;
  /* start condition for strings */
\"           { BEGIN(string); }
<string>\n   return OTHER_T;
<string>\\\" { yymore(); }
<string>\"   {
  BEGIN(INITIAL);		/* go back to normal state */
  yytext[--yyleng] = '\0';	/* remove the trailing double-quote */
  return STRINGCONST_T;
 }
<string>.    { yymore(); }

  /* Start Condition for block comments */
"/*"         { BEGIN(block_comment); }
<block_comment>[^*]*   /* Skip everything that's not a * */
<block_comment>"*"+[^*/]*      /* Skip *s not followed by a / */
<block_comment>"*/"  { BEGIN(INITIAL); } /* End the comment */



  /* Start condition for inline comments */
"//"         { BEGIN(inline_comment); }
<inline_comment>[^\n]   /* Skip everything to the new line */
<inline_comment>\n { BEGIN(INITIAL); }

","          return ',';
";"          return ';';
"="          return '=';
"+"          return '+';
"-"          return '-';
"*"          return '*';
"/"          return '/';
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

  /* Extra Credit Regular Expressions */
"+="         return PLUS_EQ_T;
"-="         return MINUS_EQ_T;
"*="         return MULT_EQ_T;
"/="         return MULT_EQ_T;
"->"         return ARROW_T;
"&"          return '&';
":"          return ':';
"."          return '.';
"?"          return '?';    /* For the ternary operator */

<<EOF>>      return EOF_T;
.            return OTHER_T;

%%

int lexLookup(char *s) {
  typedef struct combo {
    char *keyword;
    int token;
  } combo;

  const int KWCOUNT = 21;

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
      /* Extra credit keywords */
      { "break", BREAK_T },
      { "continue", CONTINUE_T },
      { "switch", SWITCH_T },
      { "case", CASE_T },
      { "default", DEFAULT_T },
      { "static", STATIC_T },
      { "char", CHAR_T },
      { "short", SHORT_T },
      { "sizeof", SIZEOF_T},
      { "struct", STRUCT_T},
      { "const", CONST_T},
      { NULL, ID_T }
    };

  combo *kw;
  
  keywords[KWCOUNT-1].keyword = s;

  for (kw = keywords; strcmp(kw->keyword, s) != 0; kw++)
    ;

  return kw->token;
}
