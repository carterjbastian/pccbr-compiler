int x[8];

int fib(int a) {
  int x, y, z;
  if(a <= 1) 
    return 1;

/*  y = fib(a);
  z = fib(a); */
//  y = fib(a-1);
//  z = fib(a-2);
  x = fib(a-1) + fib(a-2);
  return x;
}
  
int main() {
  int i;

  for (i = 1; i <=7; i=i+1) {
    x[i] = fib(i);
    // printf("%d\n", fib(i));
    print "RESULTS FOR VALUE";
    print i;
    print "IS";
    print x[i];


  }
  return 0;
}
