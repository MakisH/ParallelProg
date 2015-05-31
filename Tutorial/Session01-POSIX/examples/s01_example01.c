#include<stdio.h>
#include<stdlib.h>

void * hello()
{
	printf("Hello World from pthread!\n");
	return NULL;
}

void * bye()
{
	printf("Goodbye cruel world from another pthread!\n");
	return NULL;
}

int main()
{
	pthread_t thread_hello;
	pthread_t thread_bye;

	pthread_create(&thread_hello, NULL, &hello, NULL);
	pthread_create(&thread_bye, NULL, &bye, NULL);

	printf("Hello World from main!\n");

	pthread_join(thread_hello, NULL);
	pthread_join(thread_bye, NULL);

	return 0;
}
