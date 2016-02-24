int x = 0;

int main() {
  int y = 2;
  int i;
  for (i = 0; i < 3; ++i) {
    x = x + y;
  }
  y = x + 5;
  return y;
}

/*
 * Intended Results:
 *  x has value 6
 *  %eax has value 11 = 0xb
 */
