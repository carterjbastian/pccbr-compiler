int x[5];
/*
 * functions.c
 *
 * A test file to create various functions with various types and parameters
 */
int foo(int a[]) {
  a[1] = 7;
  return a[1];
}

int main(void) {
  int i;
  x[0] = 1;
  x[1] = 2;
  i = foo(x);
  return i;
}
