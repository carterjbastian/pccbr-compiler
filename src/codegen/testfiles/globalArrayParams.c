int x[5];
/*
 * functions.c
 *
 * A test file to create various functions with various types and parameters
 */
int foo(int a[]) {
  a[1] = 7;
  return a[2];
}

int main(void) {
  int i;
  x[0] = 1;
  x[1] = 2;
  x[2] = 3;
  x[3] = 4;
  x[4] = 5;
  i = foo(x);
  return i;
}

/*
 * Intended Results:
 *  x = {1,7,3,4,5}
 *
 *  return (%eax) = 3;
 */
