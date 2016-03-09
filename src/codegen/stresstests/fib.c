int x[8];

int fib(int a) {
  int q;
  if(a <= 1) 
    return 1;

/*  y = fib(a);
  z = fib(a); */
  q = fib(a-1) + fib(a-2);
  return q;
}
  
int main() {
  int i;

  for (i = 1; i <=3; i=i+1) {
    // printf("%d\n", fib(i));
    x[i] = fib(i);

  }
  return 0;
}
