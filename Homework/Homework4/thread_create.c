#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// Compile: gcc -o thread_create thread_create.c -lpthread

void *read_keys( void * ptr);

int main()
{
	pthread_t thread1;
	int iret1 = pthread_create( &thread1, NULL, read_keys, NULL);
	if(0 != iret1)
	{
		printf("Error - pthread_create() return code: %d\n", iret1);
		return (-1);
	}
	pthread_join(thread1, NULL);
	printf("All threads complete\n");
	return 0;
}

void *read_keys( void *ptr)
{
	printf("Enter x or X to exit\n");
	int cur_char;
	do
	{
		cur_char = getchar();
	}
	while(('x' != (char)(cur_char)) && ('X' != (char)(cur_char)));
	return 0;
}
