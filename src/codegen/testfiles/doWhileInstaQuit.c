int x = 4;

int main() {
  int y = 0;
  do {
    --x;
    y = x;
  } while (y > 5);
  return y;
}

/*
 * Intended results
 *  x = 3
 *  %eax = 3
 */
