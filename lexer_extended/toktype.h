#ifndef TOKTYPE_H_
#define TOKTYPE_H_

enum TokenType { ID_T=258, INTCONST_T, FLOATCONST_T, STRINGCONST_T,
		 ISEQUAL_T, NOTEQUAL_T, LT_EQUAL_T, GT_EQUAL_T, INCREMENT_T, 
                 DECREMENT_T, AND_T, OR_T,
                 INT_T, VOID_T, WHILE_T, FOR_T, IF_T, ELSE_T, RETURN_T,READ_T, 
                 PRINT_T, EOF_T, OTHER_T,
                 /* EXTENDED AND EXTRA CREDIT TOKEN TYPES */
                 BREAK_T, CONTINUE_T, SWITCH_T, CASE_T, DEFAULT_T, STATIC_T,
                 CHAR_T, SHORT_T, SIZEOF_T, PLUS_EQ_T, MINUS_EQ_T, MULT_EQ_T,
                 DIVIDE_EQ_T, CONST_T, STRUCT_T, ARROW_T, CHAR_LITERAL_T
                };


static char *tokens[] =
{ "ID", "INTCONST", "FLOATCONST", "STRINGCONST",
  "ISEQUAL", "NOTEQUAL", "LT_EQUAL", "GT_EQUAL", "INCREMENT", 
  "DECREMENT", "AND", "OR", "INT", "VOID", "WHILE", "FOR", "IF", "ELSE", 
  "RETURN", "READ", "PRINT", "EOF", "OTHER",
  /* Extended and extra-credit tokens */
  "BREAK", "CONTINUE", "SWITCH", "CASE", "DEFAULT", "STATIC",
  "CHAR", "SHORT", "SIZEOF", "PLUS_EQ", "MINUS_EQ", "MULT_EQ", "DIVIDE_EQ",
  "CONST", "STRUCT", "ARROW", "CHAR_LITERAL"
};

#define FIRST_BIG_TOKEN ID_T
// so that it works even if ID_T ends up nonzero

char *token_name(int token);


#endif
