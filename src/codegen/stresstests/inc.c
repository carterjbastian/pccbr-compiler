int a[3];
int b0;
int b1;
int b2;
int main(void){

  int i, n, b[3];

  b[0] = 0;
  for (i = 0; i < 3; i = i + 1) {
    a[i] = 0;
    b[i] = 0;
  }

  n = 2;

  ++a[++n]; // a[2] = 1

  n = 2;

  ++b[(++n)]; // b[2] = 1

  b0 = b[0];
  b1 = b[1];
  b2 = b[2];
  /*
  for (i = 0; i < 3; i = i + 1) {
    //    printf("a[%d] = %d, b[%d] = %d\n",i,a[i],i, b[i]);
    print a[i];
    print b[i];
  }
  */

  return b2;
}
