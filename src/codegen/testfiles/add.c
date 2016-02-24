int x = 4;
int y = 1;
int main() {
  int a = 5;
  // Add two globals
  x = x + y;
  y = x + 4;
  y = x + a;
  return (x + 1);
}
