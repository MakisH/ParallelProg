#include<stdio.h>
#include<stdlib.h>

void * triple(void *ptr) {
	int *out = malloc(sizeof(*out));
	*out = 3 * (*(int*)ptr);
	
	return out;
}

void main() {
	
	long num_threads = 3;
	pthread_t *thread;
	int *in;

	thread = malloc(num_threads * sizeof(*thread));
	in = malloc(num_threads * sizeof(*in));

	for (int i = 0; i < num_threads; i++) {
		in[i] = i;
		pthread_create(thread + i, NULL, &triple, in + i);
	}

	for (int i = 0; i < num_threads; i++) {
		int *out;
		pthread_join(thread[i], &out);
		printf("Triple of %d is %d\n",
				in[i],
				*out);
		free(out);
	}
}
