#include<stdio.h>
#include<stdlib.h>

void * hello() 
{
	printf("Hello World from pthread!\n");
	return NULL;	
}

void main()
{

	long num_threads = 3;
	pthread_t *thread;

	thread = malloc(num_threads * sizeof(*thread));

	for (int i=0; i<num_threads; i++)
		pthread_create(thread + i, NULL, &hello, NULL);

	for (int i=0; i<num_threads; i++)
		pthread_join(thread[i], NULL);

}
