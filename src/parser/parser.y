/*
 * File:    parser.y
 *
 * Authors:
 *  - Based of example files written by Tom Cormen and Sean Smith
 *  - Heavily extended, modified and mutilated for use in the pccbr compiler
 *      by Carter J. Bastian and Quinn Stearns in 2016.
 *
 * This File Contains:
 *  - The grammar for our accepted subset of the C programming language
 *  - The corresponding rules for construction of an abstract syntax tree for
 *    any input recognized by the grammar.
 *  - All error reporting and recovery functionality
 */

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

#define YYSTYPE ast_node
#define YYDEBUG 1
#define MAXTOKENLENGTH 201

// Redirected output for testing
extern FILE* error_out;

extern int yylex();
int yyerror(char *s);
extern char *yytext;

extern ast_node root;
extern int parseError;
extern int yylineno;

extern char savedIDText[];
extern char savedLiteralText[];
char savedFuncText[MAXTOKENLENGTH];

%}

%code requires {
  #include "ast.h"
  #include "symtab.h"
  #define YYSTYPE ast_node
}

/* Declarations of all of the used tokens returned from the lexer */
%token ID_T INTCONST_T STRINGCONST_T ISEQUAL_T NOTEQUAL_T LT_EQUAL_T
%token GT_EQUAL_T INCREMENT_T DECREMENT_T AND_T OR_T INT_T VOID_T
%token WHILE_T FOR_T IF_T ELSE_T RETURN_T READ_T PRINT_T EOF_T OTHER_T DO_T

/* Associativity and Precedence options */
/* We use this to ensure that errors are as specific as possible */
%nonassoc ERR_EXPR
%nonassoc ERR_PRINT ERR_READ ERR_RETURN
%nonassoc ERR_LOOP
%nonassoc ERR_IF
%nonassoc ERR_ELSE

/* This solves the if-else / dangling-else precedence problems */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_T

/* Solve other ambiguities from the original grammar */
%nonassoc NON_FUNC_DEC
%nonassoc FUNC_DEC

%nonassoc EXPR_P
%nonassoc VAR_P

%right '(' ')'
%right '[' ']'

/* Set the precedences for rValue operations */
%left OR_T
%left AND_T
%left ISEQUAL_T NOTEQUAL_T
%left '<' LT_EQUAL_T '>' GT_EQUAL_T
%left '+' '-'
%left '*' '/' '%'
%left '!' INCREMENT_T DECREMENT_T UNARYM

%%

program : declarationList {
        ast_node t = create_ast_node(ROOT_N);
        t->left_child = $1;
        root = $$ = t; }
;

declarationList : declarationList declaration { 
        ast_node t = $1;
        if (t != NULL) {
          while(t->right_sibling != NULL)
            t = t->right_sibling;
          t->right_sibling = $2; 
          $$ = $1; 
        } else {
          $$ = $2;
        } }
| declaration { $$ = $1; }
;

declaration : varDeclaration { $$ = $1; }
| funcDeclaration { $$ = $1; }
;

varDeclaration : INT_T varDeclarationList ';' %prec NON_FUNC_DEC { $$ = $2; }
| INT_T error ';' {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Invalid Variable Declaration";
        t->lineno = yylineno;
        fprintf(error_out, "PARSING ERROR:\tInvalid variable declaration discarded on line %d\n", yylineno);
        parseError++;
        $$ = t; }
;

varDeclarationList : varDeclarationList ',' varDec { 
        ast_node t = $1;
        if (t != NULL) {
          while(t->right_sibling != NULL)
            t = t->right_sibling;
          t->right_sibling = $3; 
          $$ = $1; 
        } else {
          $$ = $3;
        } }
| varDec { $$ = $1; }
;

varDec : ID_T { 
        ast_node t = create_ast_node(DEC_ID_N);
        t->value_string = strdup(savedIDText);
        t->value_int = 0;
        t->lineno = yylineno;
        $$ = t; }
| ID_T '=' expression {
        ast_node t = create_ast_node(DEC_ID_N);
        t->value_string = strdup(savedIDText);
        t->value_int = 0;
        t->lineno = yylineno;

        ast_node a = create_ast_node(OP_ASSIGN_N);
        a->left_child = t;
        a->left_child->right_sibling = $3;
        $$ = a; }
| ID_T '[' INTCONST_T ']' {
        ast_node t = create_ast_node(DEC_ID_N);
        //t->value_string = savedIDText;
        t->value_string = $1->value_string;
        t->value_int = atoi(savedLiteralText);
        t->lineno = yylineno;
        $$ = t; }
;

funcDeclaration : INT_T ID_T '(' formalParams ')' compoundStatement %prec FUNC_DEC {
        ast_node t = create_ast_node(FUNC_N);
        t->value_int = 1; // For int
        // POTENTIAL BUG: This saves ID of last id in compoundStatement, not Func name
        t->value_string = $2->value_string;
        
        t->left_child = $6;
        t->left_child->right_sibling = $4;
        t->lineno = yylineno;
        $$ = t; }
| VOID_T ID_T '(' formalParams ')' compoundStatement {
        ast_node t = create_ast_node(FUNC_N);
        t->value_int = 0; // For void
        t->value_string = $2->value_string;
        
        t->left_child = $6;
        t->left_child->right_sibling = $4;
        t->lineno = yylineno;
        $$ = t; }
;


formalParams : formalList { $$ = $1; }
| VOID_T { $$ = create_ast_node(NULL_N); $$->value_string = "void_formal_params"; }
| /* Nothing */ { $$ = create_ast_node(NULL_N); $$->value_string = "no_formal_params"; }
;

formalList : formalList ',' formalParam { 
        ast_node t = $1;
        if (t != NULL) {
          while(t->right_sibling != NULL)
            t = t->right_sibling;
          t->right_sibling = $3; 
          $$ = $1; 
        } else {
          $$ = $3;
        } }
| formalParam { $$ = $1; }
;

formalParam : INT_T ID_T {
        ast_node t = create_ast_node(PARAM_N);
        t->value_string = strdup(savedIDText);
        $$ = t; 
        t->lineno = yylineno; }
| INT_T ID_T '[' ']' { 
        ast_node t = create_ast_node(ARRAY_PARAM_N);
        t->value_string = strdup(savedIDText);
        t->lineno = yylineno;
        //t->value_string = $2->value_string;
        $$ = t; }
;

compoundStatement : '{' localDeclarations statementList '}' { 
        ast_node t = create_ast_node(COMPOUND_STMT_N);
        
        if ($2 != NULL) {
          ast_node i = $2;

          while(i->right_sibling != NULL)
            i = i->right_sibling;

          i->right_sibling = $3;
          t->left_child = $2;
        } else {
          t->left_child = $3;
        } 
        $$ = t; }
;

localDeclarations : localDeclarations varDeclaration { 
        ast_node t = $1;
        if (t != NULL) {
          while(t->right_sibling != NULL)
            t = t->right_sibling;
          t->right_sibling = $2; 
          $$ = $1; 
        } else {
          $$ = $2;
        } }
| /* Nothing */ { $$ = NULL; }
;

statementList : statementList statement {
        ast_node t = $1;
        if (t != NULL) {
          while(t->right_sibling != NULL)
            t = t->right_sibling;
          t->right_sibling = $2; 
          $$ = $1; 
        } else {
          $$ = $2;
        } }
| /* Nothing */ { $$ = NULL; }
;

statement : expressionStatement { $$ = $1; }
| compoundStatement { $$ = $1; }
| ifStatement { $$ = $1; }
| whileStatement { $$ = $1; }
| doWhileStatement { $$ = $1; }
| forStatement { $$ = $1; }
| returnStatement { $$ = $1; }
| readStatement { $$ = $1; }
| printStatement { $$ = $1; }
;

expressionStatement : expression ';' { $$ = $1; }
| error ';' %prec ERR_EXPR {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Invalid Expression";
        fprintf(error_out, "PARSING ERROR:\tStatement on line %d Discarded due to an invalid expression\n", yylineno);
        parseError++;
        $$ = t; }
| ';' { $$ = create_ast_node(NULL_N); $$->value_string = "empty expressionStatement"; }
;

ifStatement : IF_T '(' expression ')' statement   %prec LOWER_THAN_ELSE { 
        ast_node t = create_ast_node(IF_N);
        t->left_child = $3;
        t->left_child->right_sibling = $5;
        $$ = t; }
| IF_T '(' error ')' statement %prec ERR_IF {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Invalid If-Statement Condition";
        fprintf(error_out, "PARSING ERROR:\tIf-Statement discarded due to invalid condition on line %d\n", yylineno); 
        parseError++;
       $$ = t; }
| IF_T '(' expression ')' statement ELSE_T statement { 
        ast_node t = create_ast_node(IF_ELSE_N);
        t->left_child = $3;
        t->left_child->right_sibling = $5;
        t->left_child->right_sibling->right_sibling = $7;
        $$ = t; }
| IF_T '(' error ')' statement ELSE_T statement %prec ERR_ELSE {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Invalid IF-Else Condition";
        parseError++;
        fprintf(error_out, "PARSING ERROR:\tIf-Else statement discarded due to invalid condition on line %d\n", yylineno); 
        $$ = t; }

;

whileStatement : WHILE_T '(' expression ')' statement {
        ast_node t = create_ast_node(WHILE_N);
        t->left_child = $3;
        t->left_child->right_sibling = $5;
        $$ = t; }
| WHILE_T '(' error ')' statement %prec ERR_LOOP {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Invalid While-Loop Condition";
        fprintf(error_out, "PARSING ERROR:\tWhile-Loop starting on line %d discarded due to invalid condition\n", yylineno); 
        parseError++;
        $$ = t; }
;

doWhileStatement : DO_T statement WHILE_T '(' expression ')' ';' { 
        ast_node t = create_ast_node(DOWHILE_N);
        t->left_child = $5;
        t->left_child->right_sibling = $2;
        $$ = t; }
| DO_T statement WHILE_T '(' error ')' ';' %prec ERR_LOOP {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Invalid Do-While Loop Condition";
        fprintf(error_out, "PARSING ERROR:\tDo-While loop discarded due to invalid condition on line %d\n", yylineno); 
        parseError++;
        $$ = t; }
;

forStatement : FOR_T '(' forHeaderExpr ';' forHeaderExpr ';' forHeaderExpr ')' statement { 
        ast_node t = create_ast_node(FOR_N);
        t->left_child = $3;
        /* Refactor: */
        t->left_child->right_sibling = $5;
        t->left_child->right_sibling->right_sibling = $7;
        t->left_child->right_sibling->right_sibling->right_sibling = $9;
        $$ = t; }
| FOR_T '(' error ')' statement %prec ERR_LOOP {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Malformed For-Loop Header";
        fprintf(error_out, "PARSING ERROR:\tFor Loop Header discarded on line %d\n", yylineno); 
        fprintf(error_out, "\t\tNote: All statements inside loop are discarded as well\n");
        parseError++;
        $$ = t; }
;

forHeaderExpr : expression { $$ = $1; }
| /* Nothing */ { 
        $$ = create_ast_node(NULL_N); $$->value_string = "empty forHeaderExpr"; }
;

returnStatement : RETURN_T ';' { 
        ast_node t = create_ast_node(RETURN_N);
        $$ = t; }
| RETURN_T expression ';' {
        ast_node t = create_ast_node(RETURN_N);
        t->left_child = $2;
        $$ = t; }
| RETURN_T error ';' %prec ERR_RETURN {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Invalid Return Statement";
        fprintf(error_out, "PARSING ERROR:\tInvalid Return Statement discarded on line %d\n", yylineno); 
        parseError++;
        $$ = t; }
;

readStatement : READ_T var ';' { 
        ast_node t = create_ast_node(READ_N);
        t->left_child = $2;
        $$ = t; }
| READ_T error %prec ERR_READ ';' {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Malformed Read Statement";
        fprintf(error_out, "PARSING ERROR:\tMalformed Read Statement discarded on line %d\n", yylineno); 
        parseError++;
        $$ = t; }

;

printStatement : PRINT_T expression ';' { 
        ast_node t = create_ast_node(PRINT_N);
        t->left_child = $2;
        $$ = t; }
| PRINT_T STRINGCONST_T ';' { 
        ast_node t = create_ast_node(PRINT_N);
        t->left_child = create_ast_node(STRING_LITERAL_N);
        t->left_child->value_string = strdup(savedLiteralText);
        $$ = t; }
| PRINT_T error %prec ERR_PRINT ';' {
        ast_node t = create_ast_node(ERROR_N);
        t->value_string = "Malformed Print Statement";
        fprintf(error_out, "PARSING ERROR:\tMalformed Print Statement discarded on line %d\n", yylineno); 
        parseError++;
        $$ = t; }
;

expression : var '=' expression %prec EXPR_P { 
        ast_node t = create_ast_node(OP_ASSIGN_N);
        t->left_child = $1; 
        t->left_child->right_sibling = $3;
        $$ = t; }
| rValue { $$ = $1; }
;

var : ID_T %prec VAR_P { 
        ast_node t = create_ast_node(ID_N);
        t->value_string = strdup(savedIDText);
        t->value_int = 0;
        t->lineno = yylineno;
        $$ = t; }
| ID_T '[' expression ']' { 
        ast_node t = create_ast_node(ID_N);
        t->value_string = strdup(savedIDText);
        t->value_int = -1; // Denotes a complex array index
        t->left_child = $3;
        t->lineno = yylineno;
        $$ = t; }
;

rValue : expression '+' expression { 
        ast_node t = create_ast_node(OP_PLUS_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression '-' expression { 
        ast_node t = create_ast_node(OP_MINUS_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression '*' expression { 
        ast_node t = create_ast_node(OP_TIMES_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression '/' expression { 
        ast_node t = create_ast_node(OP_DIVIDE_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression '%' expression { 
        ast_node t = create_ast_node(OP_MOD_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression '<' expression { 
        ast_node t = create_ast_node(OP_LT_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression LT_EQUAL_T expression { 
        ast_node t = create_ast_node(OP_LTE_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression '>' expression { 
        ast_node t = create_ast_node(OP_GT_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression GT_EQUAL_T expression { 
        ast_node t = create_ast_node(OP_GTE_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression ISEQUAL_T expression { 
        ast_node t = create_ast_node(OP_EQUALS_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression NOTEQUAL_T expression { 
        ast_node t = create_ast_node(OP_NOTEQUALS_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression AND_T expression { 
        ast_node t = create_ast_node(OP_AND_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| expression OR_T expression { 
        ast_node t = create_ast_node(OP_OR_N);
        t->left_child = $1;
        t->left_child->right_sibling = $3;
        t->lineno = yylineno;
        $$ = t; }
| '!' expression { 
        ast_node t = create_ast_node(OP_COMPLEMENT_N);
        t->left_child = $2;
        t->lineno = yylineno;
        $$ = t; }
| '-' expression  %prec UNARYM  { 
        ast_node t = create_ast_node(OP_UNARYM_N);
        t->left_child = $2;
        t->lineno = yylineno;
        $$ = t; }
| var { $$ = $1; }
| INCREMENT_T var { 
        ast_node t = create_ast_node(OP_INCREMENT_N);
        t->left_child = $2;
        t->lineno = yylineno;
        $$ = t; }
| DECREMENT_T var { 
        ast_node t = create_ast_node(OP_DECREMENT_N);
        t->left_child = $2;
        t->lineno = yylineno;
        $$ = t; }
| '(' expression ')' { $$ = $2; }
| call { $$ = $1; }
| INTCONST_T {
        ast_node t = create_ast_node(INT_LITERAL_N);
        t->value_int = atoi(savedLiteralText);
        t->dtype = INT_LT;
        t->lineno = yylineno;
        $$ = t; }
;

call : ID_T '(' args ')' {
        ast_node t = create_ast_node(FUNC_CALL_N);
        t->value_string = $1->value_string;
        t->left_child = $3;
        t->lineno = yylineno;
        $$ = t;
     }
;

args : argList { $$ = $1; }
| /* Nothing */ { $$ = create_ast_node(NULL_N); $$->value_string = "empty arguments"; }
;

argList : argList ',' expression { 
        ast_node t = $1;
        if (t != NULL) {
          while(t->right_sibling != NULL)
            t = t->right_sibling;
          t->right_sibling = $3; 
          $$ = $1; 
        } else {
          $$ = $3;
        } }
| expression { $$ = $1; }
;


%%

/* 
 * We do not use the function for error handling.
 * we found it to be somewhat buggy and limited in terms of error handling.
 */
int yyerror(char *s) {
  return 0;
}
