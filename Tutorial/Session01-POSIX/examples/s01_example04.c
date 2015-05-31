#include<stdio.h>
#include<stdlib.h>

struct pthread_args {
	long thread_id;
	long num_threads;
};


void * hello(void *ptr) {
	struct pthread_args *arg = ptr;
	printf("Hello World from pthread %ld of %ld PID = %d TID = %d !\n",
			arg -> thread_id,
			arg -> num_threads,
			getpid(),
			(unsigned int)pthread_self());

	return NULL;	
}

void main() {

	long num_threads = 3;
	pthread_t *thread;
	struct pthread_args *thread_arg;

	thread = malloc(num_threads * sizeof(*thread));
	thread_arg = malloc(num_threads * sizeof(*thread_arg));

	for (int i=0; i<num_threads; i++) {
		thread_arg[i].thread_id = i;
		thread_arg[i].num_threads = num_threads;
		pthread_create(thread + i, NULL, &hello, thread_arg + i);
	}
	
	for (int i=0; i<num_threads; i++) {
		pthread_join(thread[i], NULL);
	}
}
