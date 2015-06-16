#include<omp.h>

void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

#pragma omp parallel for num_threads(num_threads)
for (int j = 1; j < N; j++) {
    for (int i = 1; i < N; i++) {
      c[i][j] = 3 * d[i][j];
      d[i][j]   = 2 * c[i + 1][j];
    }
}    


for (int i = 1; i < N; i++) {
    for (int j = 1; j < N; j++) {
      a[i][j]   = a[i][j] * b[i][j];
      b[i][j + 1] = 2 * a[i][j] * c[i - 1][j];
    }
  }
}