#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "histogram.h"

typedef struct {
	block_t *blocks;
	int partition_start;
	int partition_end;
} thread_args;


void * get_histogram_thread(void * arg) {

	thread_args *input = (thread_args*) arg;
	block_t *blocks = input->blocks;

	unsigned int *local_histogram = (unsigned int*)calloc(NALPHABET, sizeof(unsigned int*));

	// build histogram
	for (int i = input->partition_start; i < input->partition_end; ++i) {
		for (int j = 0; j < BLOCKSIZE; ++j) {
			if (blocks[i][j] >= 'a' && blocks[i][j] <= 'z')
				local_histogram[blocks[i][j]-'a']++;
			else if(blocks[i][j] >= 'A' && blocks[i][j] <= 'Z')
				local_histogram[blocks[i][j]-'A']++;
		}
	}

	return local_histogram;

}

void get_histogram(unsigned int nBlocks,
			 block_t *blocks,
			 histogram_t  histogram,
			 unsigned int num_threads) {

	// Partition size
	const int partition_size = (int)nBlocks/num_threads;
	// Last thread - last partition
	const int lastThread = (int)num_threads-1;

	// Array of threads
	pthread_t *thread = (pthread_t*)calloc(num_threads, sizeof(pthread_t));
	// Array of structures of thread input arguments 
	thread_args *args = (thread_args*)calloc(num_threads, sizeof(thread_args));

	// Construct the input for each thread (except the last) and start the thread. The last thread manages the case
	// of nBlocks%num_threads != 0.
	for (int i=0; i<num_threads-1; ++i) {
		args[i].blocks = blocks;
		args[i].partition_start = i*partition_size;
		args[i].partition_end = (i+1)*partition_size;
		pthread_create(thread+i, NULL, &get_histogram_thread, args+i);
	}

	// Start the last thread (managing the possibly different in size last partition) with the appropriate input.
	args[lastThread].blocks = blocks;
	args[lastThread].partition_start = lastThread*partition_size;
	args[lastThread].partition_end = nBlocks;
	pthread_create(thread+lastThread, NULL, &get_histogram_thread, args+lastThread);

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
}

