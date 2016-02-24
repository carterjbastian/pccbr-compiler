int x = 2;
int y;

void called() {
  int a = 10;
  y = a;  // Assign to the other global
}

int main() {
  int a = 2;
  x = 5; // Redefine the global
  called(); // Call the function
  return y;
}

/* End of program expectation:
 *  x = 10
 *  y = 10
 *  %eax has value 10
 */


