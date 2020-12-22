#include "Fibonacci.h"
#include <vector>

int Fibonacci::getFibonacciNumber(int n) {
  std::vector<int> res(2,1);
  if (n == 0) return 0;
  for (int i = 2; i < n; i++) {
    int back = res.back();
    int back2 = *(res.rbegin()+1);
    res.push_back(back+back2);
  }
  return res.back();
}
