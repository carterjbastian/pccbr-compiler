/*
 * expressions.c
 *
 * Testfile containing a series of complicated and exhaustive expressions
 * that are valid and should be parsed appropriately
 */
int main(void) {
  int a, b, c, d;

  a = 5 % 4 * 3 + 7 - 2 / 1;
  c = 3;
  b = (a < 5) && (c == 3) || (c > 2) && (a <= 9) || (c != 3) && !(a >= 2);
  d = - (b + (7 * 9));
  
  return (a + b) - c * d;
}
