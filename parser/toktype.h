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

static char *tokens[] =
{ "ID", "INTCONST", "STRINGCONST",
  "ISEQUAL", "NOTEQUAL", "LT_EQUAL", "GT_EQUAL", "INCREMENT", 
  "DECREMENT", "AND", "OR", "INT", "VOID", "WHILE", "FOR", "IF", "ELSE", 
  "RETURN", "READ", "PRINT", "EOF", "DO", "OTHER" };

#define FIRST_BIG_TOKEN ID_T
// so that it works even if ID_T ends up nonzero

char *token_name(int token);


#endif
