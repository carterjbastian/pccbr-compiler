%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

#define YYSTYPE ast_node
#define YYDEBUG 1

extern int yylex();
int yyerror(char *s);
extern char *yytext;

extern ast_node root;
extern int parseError;

extern char savedIdText[];
extern char savedLiteralText[];

%}

%token ID_T INTCONST_T STRINGCONST_T ISEQUAL_T NOTEQUAL_T LT_EQUAL_T
%token GT_EQUAL_T INCREMENT_T DECREMENT_T AND_T OR_T INT_T VOID_T
%token WHILE_T FOR_T IF_T ELSE_T RETURN_T READ_T PRINT_T EOF_T OTHER_T DO_T

/* This is how we fix the if-else associativity problem, as-per in class eg */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_T

%nonassoc NON_FUNC_DEC
%nonassoc FUNC_DEC

%nonassoc EXPR_P
%nonassoc VAR_P

%right '(' ')'
%right '[' ']'

/* Set the precedences for rValue operations */
/* Are all of the unary operations left-associative too? ... */
%left OR_T
%left AND_T
%left ISEQUAL_T NOTEQUAL_T
%left '<' LT_EQUAL_T '>' GT_EQUAL_T
%left '+' '-'
%left '*' '/' '%'
%left '!' INCREMENT_T DECREMENT_T UNARYM

%%

program : declarationList { }
;

declarationList : declarationList declaration { }
| declaration { }
;

declaration : varDeclaration { }
| funcDeclaration { }
;

varDeclaration : INT_T varDeclarationList ';' %prec NON_FUNC_DEC { }
;

varDeclarationList : varDeclarationList ',' varDec { }
| varDec { }
;

varDec : ID_T { }
| ID_T '=' expression { }
| ID_T '[' INTCONST_T ']' { }
;

funcDeclaration : INT_T ID_T '(' formalParams ')' compoundStatement %prec FUNC_DEC {}
| VOID_T ID_T '(' formalParams ')' compoundStatement {}
;


formalParams : formalList { }
| VOID_T { }
| /* Nothing */ { }
;

formalList : formalList ',' formalParam { }
| formalParam { }
;

formalParam : INT_T ID_T { }
| INT_T ID_T '[' ']' { }
;

compoundStatement : '{' localDeclarations statementList '}' { }
;

localDeclarations : localDeclarations varDeclaration { }
| /* Nothing */ { }
;

statementList : statementList statement { }
| /* Nothing */ { }
;

statement : expressionStatement { }
| compoundStatement { }
| ifStatement { }
| whileStatement { }
| doWhileStatement { }
| forStatement { }
| returnStatement { }
| readStatement { }
| printStatement { }
;

expressionStatement : expression ';'
| ';'
;

ifStatement : IF_T '(' expression ')' statement   %prec LOWER_THAN_ELSE { }
| IF_T '(' expression ')' statement ELSE_T statement { 
  /* NOTE: resolve ambiguity */ }
;

whileStatement : WHILE_T '(' expression ')' statement { }
;

doWhileStatement : DO_T statement WHILE_T '(' expression ')' ';' { }
;

forStatement : FOR_T '(' forHeaderExpr ';' forHeaderExpr ';' forHeaderExpr ')' statement { }
;

forHeaderExpr : expression { }
| /* Nothing */ { }
;

returnStatement : RETURN_T ';' { }
| RETURN_T expression ';' { }
;

readStatement : READ_T var ';' { }
;

printStatement : PRINT_T expression ';' { }
| PRINT_T STRINGCONST_T ';' { }
;

expression : var '=' expression %prec EXPR_P { }
| rValue { }
;

var : ID_T %prec VAR_P { }
| ID_T '[' expression ']' { }
;

rValue : expression '+' expression { /*NOTE: Add associativity & precedence */ }
| expression '-' expression { }
| expression '*' expression { }
| expression '/' expression { }
| expression '%' expression { }
| expression '<' expression { }
| expression LT_EQUAL_T expression { }
| expression '>' expression { }
| expression GT_EQUAL_T expression { }
| expression ISEQUAL_T expression { }
| expression NOTEQUAL_T expression { }
| expression AND_T expression { }
| expression OR_T expression { }
| '!' expression { }
| '-' expression  %prec UNARYM  { }
| var { }
| INCREMENT_T var { }
| DECREMENT_T var { }
| '(' expression ')' { }
| call { }
| INTCONST_T
;

call : ID_T '(' args ')' { }

;

args : argList { }
| /* Nothing */ { }
;

argList : argList ',' expression { }
| expression { }
;


%%

int yyerror(char *s) {
  parseError = 1;
  fprintf(stderr, "%s at line %d\n", s, lineNumber);
  return 0;
}
