#include <omp.h>
#include "quicksort.h"
#include "helper.h"

#define T 100

void quicksort_thread(int *a, int left, int right)
{
  if(left < right)
  {
    int x = left, y = (left+right)/2, z =right;
    int pivotIdx = (a[x] <= a[y])
        ? ((a[y] <= a[z]) ? y : ((a[x] < a[z]) ? z : x))
        : ((a[x] <= a[z]) ? x : ((a[y] < a[z]) ? z : y));

    int pivotVal = a[pivotIdx];
    swap(a + pivotIdx, a + right);

    int swapIdx = left;

    for(int i=left; i < right; i++)
    {
      if(a[i] <= pivotVal)
      {
        swap(a + swapIdx, a + i);
        swapIdx++;
      }
    }
    swap(a + swapIdx, a + right);

//     unsigned int left_size = swapIdx - 1 - left;
//     unsigned int right_size = right - swapIdx - 1;
    
    #pragma omp task final(swapIdx - 1 - left > T)
    quicksort_thread(a, left, swapIdx - 1);
    
//     #pragma omp task firstprivate(right, swapIdx) final(right - swapIdx - 1 > T)
    quicksort_thread(a, swapIdx + 1, right);

  }
}

void quicksort(int *a, int left, int right, int num_threads)
{
  
//   omp_set_dynamic(0);
//   omp_set_nested(1);
//   omp_set_max_active_levels(4);
  
  #pragma omp parallel num_threads(num_threads)
  {
    #pragma omp single nowait
    {
      quicksort_thread(a, left, right);
    }
  }
}