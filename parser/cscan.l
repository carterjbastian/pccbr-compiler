%{

#include <string.h>
#include "parser.tab.h"

int lexLookup(char *);

/* I want to be like Cormen when I grow up */
#define MAXTOKENLENGTH 201

char savedIDText[MAXTOKENLENGTH];
char savedLiteralText[MAXTOKENLENGTH];

%}

white [ \t\n]+
letter [A-Za-z]
digit [0-9]
id ({letter}|_)({letter}|{digit}|_)*
bad_id {digit}({letter}|{digit}|_)*
integer {digit}
signedint [-]?{integer}

%x string
%x block_comment
%x inline_comment

%option yylineno noyywrap
%%

{white}      /* skip whitespace */

{id}         { 
             strncpy(savedIDText, yytext, MAXTOKENLENGTH-1);
             return lexLookup(yytext); }
{signedint}  { 
             strncpy(savedLiteralText, yytext, MAXTOKENLENGTH-1);
             return INTCONST_T; }

  /* start condition for strings */
\"           { BEGIN(string); }
<string>\n   return OTHER_T;
<string>\\\" { yymore(); }
<string>\"   {
  BEGIN(INITIAL);		/* go back to normal state */
  yytext[--yyleng] = '\0';	/* remove the trailing double-quote */
  strncpy(savedLiteralText, yytext, MAXTOKENLENGTH-1);
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

{bad_id}     {
             strncpy(savedIDText, yytext, MAXTOKENLENGTH-1);
             return BAD_ID_T; }
.            return OTHER_T;

%%

int lexLookup(char *s) {
  typedef struct combo {
    char *keyword;
    int token;
  } combo;

  const int KWCOUNT = 11;

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
      { "do", DO_T },
      { NULL, ID_T }
    };

  combo *kw;
  
  keywords[KWCOUNT-1].keyword = s;

  for (kw = keywords; strcmp(kw->keyword, s) != 0; kw++)
    ;

  return kw->token;
}
