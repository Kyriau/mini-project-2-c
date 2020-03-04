#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
	int id;
	int length;
} request_t;

typedef struct {
	int id;
	pthread_t thread;
} thread_t;

void *master(void *arg) {

	printf("Master thread started.\n");

	int *nm = (int *)arg;
	int n = nm[0];
	int m = nm[1];

	clock_t t0;
	t0 = clock();

	while(1) {

		int req_dur = rand_bound(1, 1000);
		printf("%i\n", req_dur);

	}

}

void *slave(void *arg) {
	printf("slave\n");
	pthread_exit(NULL);
}

int rand_bound(int low, int high) {
	return (rand() % (high - low + 1) + low);
}

int main(int argc, char **argv) {

	// Get n and m from user
	int n, m;
	printf("How many consumer threads to you want? ");
	scanf("%i", &n);
	printf("How many seconds should the maximum request length be? ");
	scanf("%i", &m);

	// Set RNG seed
	srand(314159);

	// Generate threads;
	thread_t threads[n + 1];
	threads[0].id = 0;
	int nm[2];
	nm[0] = n;
	nm[1] = m;
	pthread_create(&threads[0].thread, NULL, master, (void *) nm);
	for(int i = 1; i < n + 1; i++) {
		threads[i].id = i;
		pthread_create(&threads[i].thread, NULL, slave, NULL);
	}

	pthread_join(&threads[0].thread, NULL);

	exit(0);

}