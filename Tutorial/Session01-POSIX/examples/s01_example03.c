#include<stdio.h>
#include<stdlib.h>

void * hello(void *ptr) 
{
	printf("Hello World from pthread %d!\n", *((int*)ptr));
	return NULL;	
}

void main()
{

	int num_threads = 3;
	pthread_t *thread;
	int *thread_arg;

	thread = malloc(num_threads * sizeof(*thread));
	thread_arg = malloc(num_threads * sizeof(*thread_arg));

	for (int i=0; i<num_threads; i++) {
		thread_arg[i] = i;
		pthread_create(thread + i, NULL, &hello, thread_arg + i);
	}
	
	for (int i=0; i<num_threads; i++) {
		pthread_join(thread[i], NULL);
	}
}
