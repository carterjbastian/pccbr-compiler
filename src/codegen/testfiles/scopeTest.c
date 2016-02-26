int in1;

int in2;

int main() {
  int x = 1;

  // Inner scope one
  {
    int x = 5;
    in1 = x;
  }

  {
    int x = 10;
    in2 = x;
  }

  return x;
}
/*
 * Intended results
 *  in1 = 0x5
 *  in2 = 0xa
 *  %eax = 0x1
 */
