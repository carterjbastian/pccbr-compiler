int main() {
  int x[10];
  int y[10];

  int i;

  for (i = 0; i < 10; ++i) {
    x[i] = i;
  }

  for (i = 0; i < 10; ++i) {
    y[i] = x[i];
  }

  return y[9];
}

/*
 * Intended results:
 *  x should be an array of ints from 0 - 9 in order
 *  y should be the same as x
 *  %eax should be 0x9
 */
