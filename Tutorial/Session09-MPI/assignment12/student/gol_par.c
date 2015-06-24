#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include "helper.h"

int np, rank;

unsigned int gol(unsigned char *grid, unsigned int dim_x, unsigned int dim_y, unsigned int time_steps)
{

  MPI_Status status;

  int* displs = (int*)malloc(np*sizeof(int));
  int* sendl = (int*)malloc(np*sizeof(int));
  
  // Set the displacements and sizes for the scatterv. 
  // The idea is that the local subgrids are allocated with
  // the extra ghost rows and the actual rows are injected in the middle.
  // The ghost rows are eitherways going to be filled at the begining of
  // each timestep.
  
  // We want to distribute only the rows.
  // The last (or the only) process takes the remainder
  // and the other processes a chunk of the same size.
  unsigned int dim_y_scat_normal = dim_y / np;
  unsigned int dim_y_scat_rem = dim_y / np + dim_y % np;
  unsigned int dim_y_local, subgridsize;

  if (rank == np - 1 ) {
    dim_y_local = dim_y_scat_rem + 2;
  } else {
    dim_y_local = dim_y_scat_normal + 2;
  }
  subgridsize = dim_y_local * dim_x;
  
  // Allocate the local subgrid
  unsigned char* subgrid = (unsigned char*)calloc(subgridsize, sizeof(unsigned char));
  
  // Build the arrays of displacements and sizes
  for (int p = 0; p < np-1; ++p) {
    displs[p] = p * dim_y_scat_normal * dim_x;
    sendl[p] = dim_y_scat_normal * dim_x;
  }
  // The last process receives the remainder
  displs[np-1] = (np-1)*dim_y_scat_normal * dim_x;
  sendl[np-1] = dim_y_scat_rem * dim_x;
  
  // Scatter the grid to the processes
  MPI_Scatterv(grid, sendl, displs, MPI_UNSIGNED_CHAR, subgrid + dim_x, dim_y_local*dim_x, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
  
  unsigned char *grid_in, *grid_out, *subgrid_tmp;
  size_t size = sizeof(unsigned char) * dim_x * dim_y_local;

  subgrid_tmp = calloc(sizeof(unsigned char), dim_y_local * dim_x);
  if (subgrid_tmp == NULL)
    exit(EXIT_FAILURE);

  grid_in = subgrid;
  grid_out = subgrid_tmp;
  
  for (int t = 0; t < time_steps; ++t) {
   

    // forward: (dim_y_local-2)-row of rank to 0-row of (rank+1+np)%np
    MPI_Sendrecv( grid_in + (dim_y_local-2)*dim_x, dim_x, MPI_UNSIGNED_CHAR, (rank+1+np)%np, 0,  
                  grid_in,                         dim_x, MPI_UNSIGNED_CHAR, (rank-1+np)%np, 0, 
                  MPI_COMM_WORLD, &status );
    // backward: 1-row of rank to to (dim_y_local-1)-row of (rank-1+np)%np
    MPI_Sendrecv( grid_in + dim_x,                 dim_x, MPI_UNSIGNED_CHAR, (rank-1+np)%np, 0,
                  grid_in + (dim_y_local-1)*dim_x, dim_x, MPI_UNSIGNED_CHAR, (rank+1+np)%np, 0, 
                  MPI_COMM_WORLD, &status );

    for (int y = 0; y < dim_y_local; ++y) {
      for (int x = 0; x < dim_x; ++x) {
        evolve(grid_in, grid_out, dim_x, dim_y_local, x, y);
      }
    }
    swap((void**)&grid_in, (void**)&grid_out);
  }

  if (subgrid != grid_in)
    memcpy(subgrid, grid_in, size);

  free(subgrid_tmp);
  
  // Gather back the subgrids to the master process
  MPI_Gatherv(subgrid + dim_x, (dim_y_local-2)*dim_x, MPI_UNSIGNED_CHAR, grid, sendl, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
  
  unsigned int retval = 0; 
  if (rank == 0) retval = cells_alive(grid, dim_x, dim_y);
  return retval;
  
}
