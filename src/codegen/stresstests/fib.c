int x[8];

int fib(int a) {
  int x;
  if(a <= 1)
    return 1;
  x = (fib(a-1) + fib(a-2));
  return x;
}
  
int main() {
  int i;

  for (i = 1; i <=7; i=i+1) {
    // printf("%d\n", fib(i));
    x[i] = fib(i);

  }
  return 0;
}
