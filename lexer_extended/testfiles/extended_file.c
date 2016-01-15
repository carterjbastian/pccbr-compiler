/*
 * A file for testing that our lexer can accept good input
 * This block comment should work.
 */
void function_name(int a, int b) {
  for (int i = 0; i < 2; i++) {
    a = a * b;
  }
}

int x = 587;
int y = 2;
if (x + y >= x % 3 || x - y  <= x * 2 && x / y == 73) {
  print("This is a string. %s\n", "String again.");
} else {
  if (!(x < 1 || x > 7 || x == 15083 || x != 1095)) {
    read(nothing, something);
  }
}

// Make an array
int z[5] = [1,2,3,4,5];
z[5]++;
z[5]--a;

// This is a non-sensical loop
while(condition) {
  // Call this silly function
  res = function_name(z[1], z[2]);
}

/* Extra Credit Testing */
char c, d;
short s, m;

while (1) {
  if (c += d == z[1] || s /= m == z[0]) {
    break;
  } else {
    if (c -= d == z[3] || s *= m == z[4]) {
      continue;
    }
  }

  switch (c) {
    case 'a':
      &z++;
    case 'b':
      &z--;
    default:
      /* do nothing */
  }
}

static s_var = 5;
const struct hi {
  int i_1;
  char c_1;
  short s_1;
} hi;

// Ternary operator
c == d ? &z++ : &z--;
