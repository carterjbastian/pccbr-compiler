#ifndef TOKTYPE_H_
#define TOKTYPE_H_

enum TokenType { ID_T=258, INTCONST_T, FLOATCONST_T, STRINGCONST_T,
		 COMMA_T, SEMICOLON_T, ASSIGN_T, PLUS_T, MINUS_T, ASTERISK_T,
		 SHARP_T, LESS_T, GREATER_T, ISEQUAL_T, NOTEQUAL_T, LT_EQUAL_T, GT_EQUAL_T,
         INCREMENT_T, DECREMENT_T,
         AND_T, OR_T,
		 SHL_T, DOT_T, ARROW_T,
		 INCLUDE_T, EXTERN_T,
         INT_T, CHAR_T, VOID_T,
         WHILE_T, FOR_T, IF_T, ELSE_T, RETURN_T,
         READ_T, PRINT_T,
		 EOF_T, OTHER_T };


static char *tokens[] =
{ "ID", "INTCONST", "FLOATCONST", "STRINGCONST",
  "COMMA", "SEMICOLON", "ASSIGN", "PLUS", "MINUS", "ASTERISK",
  "SHARP", "LESS", "GREATER", "ISEQUAL", "NOTEQUAL", "LT_EQUAL", "GT_EQUAL",
  "INCREMENT", "DECREMENT",
  "AND", "OR",
  "SHL", "DOT", "ARROW",
  "INCLUDE", "EXTERN",
  "INT", "CHAR", "VOID",
  "WHILE", "FOR", "IF", "ELSE", "RETURN",
  "READ", "PRINT",
  "EOF", "OTHER" };

#define FIRST_BIG_TOKEN ID_T
// so that it works even if ID_T ends up nonzero

char *token_name(int token);


#endif
