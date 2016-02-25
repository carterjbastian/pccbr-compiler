int x = -1;
int y = -1;

int hasParam(int a, int b) {
  int c = 1;
  x = a;
  y = b;
  return c;
}

int main() {
  int d;
  int newX = 5;
  int newY = 10;
  d = hasParam(newX, newY);
  return d;
}

/*
 * Intended Results at end of program:
 *  x = 0x5
 *  y = 0xc
 *  %eax = 0x1
 */
