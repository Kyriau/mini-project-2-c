#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _request_t {
	int id;
	int length;
} request_t;

typedef struct _thread_t {
	int id;
	pthread_t thread;
} thread_t;

void *master(int n, int m) {

}

void *slave() {

}

int main(int argc, char **argv) {

	// Get n and m from user
	int n, m;
	printf("How many consumer threads to you want? ");
	scanf("%i", &n);
	printf("How many seconds should the maximum request length be? ");
	scanf("%i", &m);

	// Generate thread ids;
	thread_t threads[n + 1];
	for(int i = 0; i < n + 1; i++) {
		threads[i].id = i;
	}

	exit(0);

}