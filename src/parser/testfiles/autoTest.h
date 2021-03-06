/*
 * autoTest.h   A file containing information for autogenerated parser testing
 *
 * Authors:
 *  Carter J. Bastian and Quinn Stearns, for CS57 in 2016
 *
 * This File Contains:
 *
 */
#ifndef _AUTOTEST_H_
#define _AUTOTEST_H_

#define TEST_COUNT 7
#define FILE_NAME_LEN 50

#define TEST_EXT ".c"
#define TEST_PATH "./testfiles/"

#define OUT_EXT ".out"
#define OUT_PATH "./outfiles/"

// The number of errors that should be found in our errTest file
#define REPORTED_ERROR_COUNT  11
/*
 * Note: It is important that "errTest" be listed last
 *    in order for error report and recovery testing to work
 */
static char* test_names[] = {
  "forLoop",
  "loops",
  "generalTest",
  "expressions",
  "controlFlow",
  "functions",
  "errTest"
};


#endif // _AUTOTEST_H_
