#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	int id;
	int length;
} request_t;

typedef struct {
	int id;
	pthread_t thread;
} thread_t;

typedef struct {
	int n;
	int m;
} master_args_t;

void *master(void *arg) {
	
}

void *slave(void *arg) {

}

int main(int argc, char **argv) {

	// Get n and m from user
	master_args_t nm;
	printf("How many consumer threads to you want? ");
	scanf("%i", &nm.n);
	printf("How many seconds should the maximum request length be? ");
	scanf("%i", &nm.m);

	// Generate threads;
	thread_t threads[n + 1];
	threads[0].id = 0;
	pthread_create(&threads[0].thread, NULL, master, (void *) &nm);
	for(int i = 1; i < n + 1; i++) {
		threads[i].id = i;
		pthread_create(&threads[i].thread, NULL, slave, NULL);
	}

	exit(0);

}