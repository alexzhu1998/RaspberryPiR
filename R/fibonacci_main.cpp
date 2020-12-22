#include <Rcpp.h>
using namespace Rcpp;

#include "Fibonacci.h"

// [[Rcpp::export]]
int fibonacci_main(int x) {
  Fibonacci fib;
  return fib.getFibonacciNumber(x);
}




/*** R
fibonacci_main(10)
*/
