int x = 0;
int y = 0;

void inner() {
  int b = 5;
  x = b;
}

void outer() {
  int a = 10;
  y = a;
  inner();
}

int main() {
  x = 2;
  y = 2;
  outer();
}
