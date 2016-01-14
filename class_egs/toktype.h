#ifndef TOKTYPE_H_
#define TOKTYPE_H_

enum TokenType { ID_T=258, INTCONST_T, FLOATCONST_T, STRINGCONST_T,
		 SEMICOLON_T, ASSIGN_T, PLUS_T, MINUS_T, ASTERISK_T,
		 SHARP_T, LESS_T, GREATER_T, ISEQUAL_T, NOTEQUAL_T,
		 SHL_T, DOT_T, ARROW_T,
		 INCLUDE_T, EXTERN_T, INT_T, CHAR_T, WHILE_T, FOR_T,
		 EOF_T, OTHER_T };


static char *tokens[] =
{ "ID", "INTCONST", "FLOATCONST", "STRINGCONST",
  "SEMICOLON", "ASSIGN", "PLUS", "MINUS", "ASTERISK",
  "SHARP", "LESS", "GREATER", "ISEQUAL", "NOTEQUAL",
  "SHL", "DOT", "ARROW",
  "INCLUDE", "EXTERN", "INT", "CHAR", "WHILE", "FOR",
  "EOF", "OTHER" };

#define FIRST_BIG_TOKEN ID_T
// so that it works even if ID_T ends up nonzero

char *token_name(int token);


#endif
