/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 43 "parser.y" /* yacc.c:1915  */

  #include "ast.h"
  #define YYSTYPE ast_node

#line 49 "parser.tab.h" /* yacc.c:1915  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID_T = 258,
    INTCONST_T = 259,
    STRINGCONST_T = 260,
    ISEQUAL_T = 261,
    NOTEQUAL_T = 262,
    LT_EQUAL_T = 263,
    GT_EQUAL_T = 264,
    INCREMENT_T = 265,
    DECREMENT_T = 266,
    AND_T = 267,
    OR_T = 268,
    INT_T = 269,
    VOID_T = 270,
    WHILE_T = 271,
    FOR_T = 272,
    IF_T = 273,
    ELSE_T = 274,
    RETURN_T = 275,
    READ_T = 276,
    PRINT_T = 277,
    EOF_T = 278,
    OTHER_T = 279,
    DO_T = 280,
    ERR_EXPR = 281,
    ERR_PRINT = 282,
    ERR_READ = 283,
    ERR_RETURN = 284,
    ERR_LOOP = 285,
    ERR_IF = 286,
    ERR_ELSE = 287,
    LOWER_THAN_ELSE = 288,
    NON_FUNC_DEC = 289,
    FUNC_DEC = 290,
    EXPR_P = 291,
    VAR_P = 292,
    UNARYM = 293
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
