#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "helper.h"

void reverse(char *str, int strlen)
{
	
	int np, rank;
  int substrlen;
  MPI_Status status;

  // Initialize MPI
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int* displs = (int*)malloc(np*sizeof(int));
  int* sendl = (int*)malloc(np*sizeof(int));
  
  // Calculate the lenth of the substring for the "normal" case and the remaining last process
  int substrlen_normal = strlen / np;
  int substrlen_rem = substrlen_normal + strlen%np;
  
  if (rank == np - 1) {
    substrlen = substrlen_rem;
  } else {
    substrlen = substrlen_normal;
  }
  char* substr = (char*)malloc(substrlen*sizeof(char));  
  
  // Build the array of displacements and sizes for the Scatterv
  for (int p = 0; p < np-1; ++p) {
    displs[p] = p*substrlen_normal;
    sendl[p] = substrlen_normal;
  }
  displs[np-1] = (np-1)*substrlen_normal;
  sendl[np-1] = substrlen_rem;
  
  // Scatter the string to the processes
  MPI_Scatterv(str, sendl, displs, MPI_CHAR, substr, substrlen, MPI_CHAR, 0, MPI_COMM_WORLD);
  
  // Reverse the local substring
  reverse_str(substr, substrlen);
  
  // The slaves send, the master receives. We could just use a Gatherv but it is not allowed.
  if (rank != 0) {
    // Send the result substring back to the master process
    MPI_Send(substr, substrlen, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  } else {
    // Receive the substr of the last process first
    char* buffer_substr = (char*)malloc(substrlen_rem);
    MPI_Recv(buffer_substr, substrlen_rem, MPI_CHAR, np-1, 0, MPI_COMM_WORLD, &status);
    
    int str_index = 0;
    for (int i = 0; i < substrlen_rem; i++) {
      str[str_index] = buffer_substr[i];
      str_index++;
    }
        
    // Receive the substr of the other slave processes
    for (int p = np-2; p > 0; --p) {
      MPI_Recv(buffer_substr, substrlen_normal, MPI_CHAR, p, 0, MPI_COMM_WORLD, &status);
      for (int i = 0; i < substrlen_normal; ++i) {
        str[str_index] = buffer_substr[i];
        str_index++;
      }
    }
    
    // Add the master process' substring
    for (int i = 0; i < substrlen_normal; ++i) {
      str[str_index] = substr[i];
      str_index++;
    }
    
  }
    
}
