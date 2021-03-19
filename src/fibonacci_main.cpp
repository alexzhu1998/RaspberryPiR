#include "Fibonacci.h"
extern "C" {
#include <wiringPi.h>
}
#include <stdio.h>
#include <R.h>
#include <Rdefines.h>
#include <vector>


// int fibonacci_main(int x) {
//   Fibonacci fib;
//   return fib.getFibonacciNumber(x);
// }

// void double_me(int x) {
//   // Doubles the value at the memory location pointed to by x
//   x = x + x;
// }

extern "C" {
  void wiringPiSetup() {
    wiringPiSetup();
  }
}

extern "C" {
  void addOneToVector(int* n, double* vector) {
    for (int i = 0; i < *n; ++i)
      vector[i] += 1.0;
  }
}
extern "C" {
  SEXP helloA1() {
    printf("hello World\n");
    return (R_NilValue);
  }
}

extern "C" SEXP conv(SEXP a, SEXP b) {
  int i, j, na, nb, nab;
  double *xa, *xb, *xab;
  SEXP ab;
  PROTECT(a = AS_NUMERIC(a));
  PROTECT(b = AS_NUMERIC(b));
  na = LENGTH(a);
  nb = LENGTH(b);
  nab = na +nb -1;
  PROTECT(ab = NEW_NUMERIC(nab));
  xa = NUMERIC_POINTER(a);
  xb = NUMERIC_POINTER(b);
  xab = NUMERIC_POINTER(ab);
  for (i = 0; i < nab; i++) {
    xab[i] = 0.0;
  }
  for (i = 0; i < na; i++) {
    for (j = 0; j < nb; j++) {
      xab[i+j] += xa[i]*xb[j];
    }
  }
  UNPROTECT(3);
  return(ab);
}

// SEXP add(SEXP a, SEXP b) {
//   SEXP result = PROTECT(allocVector(REALSXP, 1));
//   REAL(result)[0] = asReal(a) + asReal(b);
//   UNPROTECT(1);
//
//   return result;
// }
