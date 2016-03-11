int a[4];
int main(void){

  int i, n, b[4];

  b[0] = 0;
  for (i = 0; i < 4; i = i + 1) {
    a[i] = 5;
    b[i] = 5;
  }

  n = 1;

//  ++a[++n]; // a[2] = 1 
  print "before";
  a[--n] = a[n] - 1;
  n = 1;

  --b[(--n)]; // b[2] = 1

  for (i = 0; i < 4; i = i + 1) {
    //    printf("a[%d] = %d, b[%d] = %d\n",i,a[i],i, b[i]);
    print a[i];
    print b[i];
  }

  return b[2];
}
