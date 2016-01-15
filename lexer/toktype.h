/*
 * File: toktype.h
 *  A file with the definitions of tokens use in the pccbr compiler.
 *
 * Authors: Carter J. Bastian, Quinn Stearns
 *    COSC57, 15W
 *
 * Special Considerations:
 *  - Based heavily on a similar file written by Thomas H. Cormen and Sean Smith
 *
 */
#ifndef TOKTYPE_H_
#define TOKTYPE_H_

enum TokenType { ID_T=258, INTCONST_T, FLOATCONST_T, STRINGCONST_T,
		 ISEQUAL_T, NOTEQUAL_T, LT_EQUAL_T, GT_EQUAL_T, INCREMENT_T, 
                 DECREMENT_T, AND_T, OR_T,
                 INT_T, VOID_T, WHILE_T, FOR_T, IF_T, ELSE_T, RETURN_T,READ_T, 
                 PRINT_T, EOF_T, OTHER_T };


static char *tokens[] =
{ "ID", "INTCONST", "FLOATCONST", "STRINGCONST",
  "ISEQUAL", "NOTEQUAL", "LT_EQUAL", "GT_EQUAL", "INCREMENT", 
  "DECREMENT", "AND", "OR", "INT", "VOID", "WHILE", "FOR", "IF", "ELSE", 
  "RETURN", "READ", "PRINT", "EOF", "OTHER" };

#define FIRST_BIG_TOKEN ID_T
// so that it works even if ID_T ends up nonzero

char *token_name(int token);


#endif
