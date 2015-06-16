#include<omp.h>

void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

  omp_set_num_threads(num_threads);
  
  #pragma omp parallel for
	for (int i = 1; i < N; i++) {
    a[i][1] = 3 * b[i][1];
    c[i][0] = a[i][1] * d[i][1];
		for (int j = 2; j < N; j++) {
      b[i][j] = c[i][j-1] * c[i][j-1];      
      a[i][j] = 3 * b[i][j];
      c[i][j - 1] = a[i][j] * d[i][j];
		}
		b[i][N] = c[i][N-1] * c[i][N-1];
	}
}