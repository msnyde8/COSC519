#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// Compile: gcc -o multi_thread_create multi_thread_create.c -lpthread

void *print_hello( void * ptr);

int main()
{
	printf("Main thread: %u\n", pthread_self());
	pthread_t tID[20];

	unsigned int i = 0;
	for(i = 0; i < 20; ++i)
	{
		//int iret1 = pthread_create(&tID[i], NULL, print_hello, (void *)(tID[i]));
		int iret1 = pthread_create(&tID[i], NULL, print_hello, NULL);
		if(0 != iret1)
		{
			printf("Error - pthread_create() return code: %d\n", iret1);
			return (-1);
		}
		//printf("tID: %u\n", tID[i]);
	}
	for(i = 0; i < 20; ++i)
	{
		pthread_join(tID[i], NULL);
	}
	printf("All threads complete\n");
	return 0;
}

void *print_hello( void *ptr)
{
	//pthread_t tid = (pthread_t)ptr;
	//printf("Hello from thread %u\n", tid);
	printf("Hello from thread %u\n", pthread_self());

	return 0;
}
