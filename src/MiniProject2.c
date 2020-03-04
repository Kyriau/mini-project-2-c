#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

int n, m;

typedef struct {
	int id;
	struct timespec dur;
} request_t;

typedef struct {
	int id;
	pthread_t thread;
} thread_t;

int rand_bound(int low, int high) {
	return rand() % (high - low + 1) + low;
}

long rand_nanos() {
	return (rand_bound(1, 1000) - 1) * 1000000l
		+ (rand_bound(1, 1000) - 1) * 1000l
		+ (rand_bound(1, 1000) - 1);
}

double secs(struct timespec t) {
	return (double) t.tv_sec + (double)t.tv_nsec / 1000000000.0;
}

void push(request_t req, request_t *queue) {

}

request_t pop() {
	request_t result;
	return result;
}

void *master(void *arg) {

	printf("Master thread started.\n");

	request_t *queue = (request_t *)arg;

	while(1) {

		request_t req;
		req.dur.tv_sec = rand_bound(1, m) - 1;
		req.dur.tv_nsec = rand_nanos();
		//add(req);
		printf("Producer: produced request id=%i of length %fs\n", req.id, secs(req.dur));

		// Sleep for between 0 and 999999999 nanoseconds
		struct timespec sleep_dur;
		sleep_dur.tv_nsec = rand_nanos();
		printf("Producer: sleeping for %fs\n", (double)(sleep_dur.tv_nsec/1000000000.0));
		nanosleep(&sleep_dur, &sleep_dur);

	}

	pthread_exit(NULL);

}

void *slave(void *arg) {
	printf("slave\n");
	pthread_exit(NULL);
}

int main(int argc, char **argv) {

	// Get n and m from user
	printf("How many consumer threads to you want? ");
	scanf("%i", &n);
	printf("How many seconds should the maximum request length be? ");
	scanf("%i", &m);

	// Set RNG seed
	srand(314159);

	// Allocate queue
	request_t queue[n];

	// Generate threads;
	thread_t threads[n + 1];
	threads[0].id = 0;
	pthread_create(&threads[0].thread, NULL, master, &queue);

	for(int i = 1; i < n + 1; i++) {
		threads[i].id = i;
		pthread_create(&threads[i].thread, NULL, slave, &queue);
	}

	// Hold until user input (temporary)
	char *cmd;
	scanf(" %c", cmd);
	//pthread_join(threads[0].thread, NULL);

	exit(0);

}