int x = 10;

int main() {
  int y = 10;
  do {
    --x;
    y = x;
  } while (y > 5);
  return y;
}

/*
 * Intended results
 *  x = 5
 *  %eax = 5
 */
