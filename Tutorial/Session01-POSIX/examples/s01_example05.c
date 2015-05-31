#include<stdio.h>
#include<stdlib.h>

struct pthread_args {
	int in, out;
};

void * triple(void *ptr) {
	struct pthread_args *arg = ptr;
	
	arg->out = 3 * arg->in;

	return NULL;
}

void main() {
	
	long num_threads = 3;
	pthread_t *thread;
	struct pthread_args *thread_arg;

	thread = malloc(num_threads * sizeof(*thread));
	thread_arg = malloc(num_threads * sizeof(*thread_arg));

	for (int i = 0; i < num_threads; i++) {
		thread_arg[i].in = i;
		pthread_create(thread + i, NULL, &triple, thread_arg + i);
	}

	for (int i = 0; i < num_threads; i++) {
		pthread_join(thread[i], NULL);
		printf("Triple of %d is %d\n",
				thread_arg[i].in,
				thread_arg[i].out);
	}
}
