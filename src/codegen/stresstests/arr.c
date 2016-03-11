int glob[3];
int check1;
int check2;
int check3;

void fun2(int a[]) {

  a[1] = 17;

}

int fun1(int a[]) {

  int c[3];

  c[1] = 0;
  fun2(c);

  print "c[1]: (should be 17)";
  print c[1];
  check1 = c[1]; // 
  //printf("%d\n", c[1]);
  
  a[2] = 23;

  return 32;

  
}



int main (void) {

  int la[3];
  int y;

  y = fun1(glob);

  print "glob[2]";
  print glob[2];
  check2 = glob[2];
  //printf("%d\n", glob[2]);  
 
  y = fun1(la);

  check3 = la[2];
  print "ls[2]";
  print la[2];
  //printf("%d\n", la[2]);    

  return y;
}

/*
 * check1 = 17
 * check2 = 23
 * check3 = 23
 * glob[2] = 23;
 *
 * return = 32;
 */
