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
  int x[3];
  x[0] = 1;
  x[1] = 2;
  x[2] = 3;
  i = foo(x);

  print x[0];
  print x[1];
  print x[2];

  return x[1];
}

/*
 * Intended results:
 *  Return (%eax) = 5
 */
