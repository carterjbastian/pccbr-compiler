int x[10];
int y[10];

int main() {
  int i = 0;

  /*for (i = 0; i < 10; i++) {
    x[i] = i;
  } */
  x[i + 1] = 10; // x[1] = 10
  //y[1] = x[1];
  x[2] = x[i + 1]; // x[2] = 10
  i = x[2];

  return i;
}

/*
 * Intended results:
 *  x[1] = 0xa
 *  x[2] = 0xa
 *  %eax = 0xa
 */
