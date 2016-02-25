int x = 9;
int y = 2;

int add;
int sub;
int div;
int mull;
int mod;
int multi_term;

int main() {
  // Add two globals
  add = x + y;    // Should be 11 = 0xb
  sub = x - y;    // Should be 7
  div = x / y;    // Should be 4
  mull = x * y;   // Should be 18 = 0x12
  mod = x % y;    // Should be 1

  multi_term = ((((mod + sub) / div) * add) % mull); // Should be 4
  return multi_term;
}
