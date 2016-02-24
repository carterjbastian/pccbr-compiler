int saved(int x) {
  int b = x * 3;
  --b;
  return b;
}

int main() {
  int y = 0;
  int a = 2;
/*  y = saved(a); // y = 5
  ++y;
  return y; // 6 should be in %eax */
  return saved(a);
}
