#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "histogram.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	char *buffer;
  unsigned int chunk_size;
  int *start;
  int *end_of_file;
  //char *padding[64];
} thread_args;

void * get_histogram_thread(void * arg) {

  thread_args  *input = (thread_args*) arg;
	unsigned int *local_histogram = (unsigned int*)calloc(NALPHABET, sizeof(unsigned int*));
  int start;
  int end_of_file = 0;

  // While there is a part of the file that isn't processed yet
  while ( 1 ) {
    // Pick the next unassigned chunk
    // This read/write has to be protected.
    pthread_mutex_lock(&mutex);
    end_of_file = input->end_of_file[0];
      if ( end_of_file == 1 ) {
        pthread_mutex_unlock(&mutex);
        break;
      }
      input->start[0] += input->chunk_size;
      start = input->start[0];
      //printf("Start = %d \n", start);
    pthread_mutex_unlock(&mutex);

    // Traverse all the chunk and build the local histogram
    int i=0;
    for (i = start; i < start + input->chunk_size && input->buffer[i] != TERMINATOR; i++) {
      // lowercase
	    if (input->buffer[i] >= 'a' && input->buffer[i] <= 'z')
		    local_histogram[input->buffer[i]-'a']++;
      // uppercase
	    else if(input->buffer[i] >= 'A' && input->buffer[i] <= 'Z')
		    local_histogram[input->buffer[i]-'A']++;
    } // end for

    if (input->buffer[i-1] == TERMINATOR) {
      pthread_mutex_lock(&mutex);
        input->end_of_file[0] = 1;
      pthread_mutex_unlock(&mutex);
    }

  } // end while

  return local_histogram;

}

void get_histogram(char *buffer,
		   			 unsigned int* histogram,
		   			 unsigned int num_threads,
						 unsigned int chunk_size) {

	// Array of threads
	pthread_t *thread = (pthread_t*)calloc(num_threads, sizeof(pthread_t));
	// Array of structures of thread input arguments 
	thread_args *args = (thread_args*)calloc(num_threads, sizeof(thread_args));  

  int * start = (int*)calloc(1,sizeof(int*));
  *start = - chunk_size;

  int * end_of_file = (int*)calloc(1,sizeof(int*));
  *end_of_file = 0;

  // Construct the input for each thread and start the thread
  for (int i = 0; i < num_threads; i++) {
    args[i].buffer = buffer;
    args[i].chunk_size = chunk_size;
    args[i].start = start;
    args[i].end_of_file = end_of_file;
		pthread_create(thread+i, NULL, &get_histogram_thread, args+i);
  } 

	// Join the threads
	for (int i=0; i<num_threads; ++i) {
		unsigned int *local_histogram;
		pthread_join(thread[i], (void**) &local_histogram);
		// Sum the different histograms
		for (int j=0; j<NALPHABET; ++j) {
			histogram[j] 	+= local_histogram[j];
		}
		free (local_histogram);
	}
  
	// Free the dynamically allocated memory
	free (thread);
	free (args);
  free (start);
  free (end_of_file);


}
