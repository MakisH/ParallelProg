#include <omp.h>

#include "quicksort.h"
#include "helper.h"

#define T 20

void quicksort_sequential(int *a, int left, int right)
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

    quicksort_sequential(a, left, swapIdx - 1);
    quicksort_sequential(a, swapIdx + 1, right);
    
  }
}

void quicksort_thread(int *a, int left, int right)
{
  
  if(left < right)
  {
    
    if( right-left <= T ) {
      quicksort_sequential(a, left, right);
      return;
    }
    
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

    #pragma omp parallel sections
    {

      quicksort_thread(a, left, swapIdx - 1);

      #pragma omp section
      {
        quicksort_thread(a, swapIdx + 1, right);
      }
    }
  }
  
}

void quicksort(int *a, int left, int right, int num_threads)
{
  
  omp_set_dynamic(0);
  omp_set_nested(1);
  omp_set_max_active_levels(5);
  omp_set_num_threads(num_threads);
  
  #pragma omp parallel
  {
    #pragma omp single
    {
      quicksort_thread(a, left, right);
    }      
  }
}
