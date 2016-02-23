int x = 5;
int y;

void called(int a) {
  a = x;  // Set the parameter equal to the global
  y = a;  // Assign to the other global
  a = 0;  // Assign to the parameter (NOT the global)
}

int main() {
  x = 10; // Redefine the global
  called(x); // Call the function
  return y;
}

/* End of program expectation:
 *  x = 10
 *  y = 10
 *  %eax has value 10
 */


