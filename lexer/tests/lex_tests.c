/*
 * File: lex_tests.c
 *  A file containing definitions of all of the information needed for automated
 *  testing of our scanner.
 *
 * Authors: Carter J. Bastian, Quinn Stearns
 *    COSC57, 15W
 *
 * Special Considerations:
 *
 */
#include "../toktype.h"
#include "lex_tests.h"

const char *test_files[] = { "tests/ID_T.test",
                              "tests/INTCONST_T.test",
                              "tests/STRINGCONST_T.test",
                              "tests/EQUALITIES_T.test",
                              "tests/INCREMENT_DECREMENT_T.test",
                              "tests/LOGIC_OPS.test",
                              "tests/CONTROL_FLOW.test",
                              "tests/MISC_KEYWORDS.test"
                            };

const result_node test_comparison[] = { 
      { ID_T, ID_T, ID_T, ID_T, ID_T },
      { INTCONST_T, INTCONST_T, INTCONST_T, INTCONST_T, INTCONST_T },
      { STRINGCONST_T, STRINGCONST_T, STRINGCONST_T, STRINGCONST_T, STRINGCONST_T },
      { ISEQUAL_T, LT_EQUAL_T, GT_EQUAL_T, NOTEQUAL_T, '='},
      { INCREMENT_T, DECREMENT_T, '+', '+', '-' },
      { AND_T, OR_T, OTHER_T, OTHER_T, OTHER_T },
      { WHILE_T, FOR_T, IF_T, ELSE_T, RETURN_T },
      { INT_T, VOID_T, READ_T, PRINT_T, ID_T }
};
