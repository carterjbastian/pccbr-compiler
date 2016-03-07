int x;

int check1;
int check2;
int check3;
void sub(int x) {

  x = 3;

}

int main(void) {

  x = 1;

  // printf("%d\n", x);
  check1 = x; // should be 1

  {
    int x;

    x = 2; 
    sub(x);

    check2 = x; // should be 2
  }

  check3 = x;  // should still be 1

  return 0;

}
