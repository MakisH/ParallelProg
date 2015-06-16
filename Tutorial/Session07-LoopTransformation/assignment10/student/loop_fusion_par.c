#include<omp.h>

void compute(unsigned long **a, unsigned long **b, unsigned long **c, unsigned long **d, int N, int num_threads) {

  omp_set_num_threads(num_threads);

  #pragma omp parallel for
	for (int i = 1; i < N; i++) {
		for (int j = 1; j < N+1; j++) {
			if(j<N) a[i][j] = 2 * b[i][j];
			if(j<N) d[i][j] = a[i][j] * c[i][j];
      if(j>2) c[i][j - 2] = a[i][j - 2] - a[i][j];
		}
	}
}
