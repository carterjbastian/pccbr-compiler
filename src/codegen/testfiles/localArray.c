int idx = 2;

int main() {
  int x[10];
  int i = 1;

  x[0] = 1;     //x[0] <- 1
  x[i] = 2;     //x[1] <- 2
  x[i+1] = x[0] + x[i];  // x[2] <- 1 + 2 
  return x[idx];  // returns 3
}

/*
 * Intended results
 *  x[0] = 1
 *  x[1] = 2
 *  x[2] = 3
 *  %eax = 3
 */
