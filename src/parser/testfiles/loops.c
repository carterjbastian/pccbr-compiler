int main(void) {
  int i;
  for (i = 0; i < 100; ++i) {
    while(i < 50) {
      do {
        i = i - 1;
      } while (i < 0);
      i = i + 2;
    }
  }
  return i;
}
