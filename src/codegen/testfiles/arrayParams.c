/*
 * functions.c
 *
 * A test file to create various functions with various types and parameters
 */
int foo(int a[]) {
  a[1] = 5;
  return a[1];
}

int main(void) {
  int i;
  int x[5];
  x[0] = 1;
  x[1] = 2;
  i = foo(x);
  return x[1];
}

/*
 * Intended results:
 *  Return (%eax) = 5
 */
