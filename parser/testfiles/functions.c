/*
 * functions.c
 *
 * A test file to create various functions with various types and parameters
 */
int foo(int a, int b, int c, int d) {
  return a;
}

void boo(int scared) {
  print scared;
}

int zoo() {
  return 5;
}

int noo(void) {
  return 6;
}

void too(void) {
  print "Also";
}

void loo() {
  print "Lou";
}

int main(void) {
  foo(1,2,3,4);
  boo(5);
  zoo();
  noo();
  too();
  loo();
  return 0;
}
