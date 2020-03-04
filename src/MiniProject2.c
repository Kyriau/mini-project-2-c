#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// Input parameters
int n, m;

// Unique ID generators
int id_count = 0; // Request IDs
int cons_id_count = 0; // Consumer IDs

// Queue control
int queue_count, queue_space;

// Semaphores
// mutex - Basic lock
// sem_prod - Producer waits while the queue is full
// sem_cons - Consumers wait while the queue is empty
sem_t mutex, sem_prod, sem_cons;

// struct for requests
typedef struct {
	int id;
	struct timespec dur;
} request_t;

// Two random generation helper funcitons
int rand_bound(int low, int high) {
	return rand() % (high - low + 1) + low;
}

long rand_nanos() {
	return (rand_bound(1, 1000) - 1) * 1000000l
		+ (rand_bound(1, 1000) - 1) * 1000l
		+ (rand_bound(1, 1000) - 1);
}

// Convert timespec struct to floating point for prints
double secs(struct timespec t) {
	return (double)t.tv_sec + (double)t.tv_nsec / 1000000000.0;
}

// Function for producer thread
void *master(void *arg) {

	// Exctract queue from arg
	request_t (*queue)[n] = (request_t (*)[n])arg;

	// Loop forever
	while(1) {

		printf("Producer: waiting for free space in the request queue.\n");
		
		// Accquire lock
		sem_wait(&mutex);

		// Wait until the queue has space
		while(queue_space == 0) {

			// Keep semaphore use outside of lock
			sem_post(&mutex);
			sem_wait(&sem_prod);
			sem_wait(&mutex);

		}

		// Produce a new random request
		int req_id = id_count++;
		struct timespec req_dur;
		req_dur.tv_sec = rand_bound(1, m) - 1;
		req_dur.tv_nsec = rand_nanos();
		printf("Producer: produced request id=%i of length %fs.\n", req_id, secs(req_dur));
		
		// Add new request to queue
		queue_space--;
		(*queue)[queue_count % n].id = req_id;
		(*queue)[queue_count++ % n].dur = req_dur;

		// Release lock
		sem_post(&mutex);

		// Notify consumers
		sem_post(&sem_cons);

		// Sleep for between 0 and 999999999 nanoseconds
		struct timespec sleep_dur;
		sleep_dur.tv_nsec = rand_nanos();
		printf("Producer: sleeping for %fs.\n", (double)(sleep_dur.tv_nsec/1000000000.0));
		nanosleep(&sleep_dur, &sleep_dur);

	}

	pthread_exit(NULL);

}

// Function for consumer thread
void *slave(void *arg) {

	// Exctract queue from arg
	request_t (*queue)[n] = (request_t (*)[n])arg;

	// Generate ID for this consumer
	int id = cons_id_count++;

	// Loop forever
	while(1) {

		printf("Consumer id=%i: Waiting for request to be available.\n", id);
		
		// Accquire lock
		sem_wait(&mutex);

		// Wait for requests to be available in the queue
		while(queue_space == n) {

			// Keep semaphore use outside of lock
			sem_post(&mutex);
			sem_wait(&sem_cons);
			sem_wait(&mutex);

		}

		// Fetch request from queue
		request_t req = (*queue)[(queue_count + queue_space++) % n];

		printf("Consumer id=%i: Handling request id=%i, length=%fs.\n", id, req.id, secs(req.dur));

		// Release lock
		sem_post(&mutex);

		// Signal producer
		sem_post(&sem_prod);

		// Spend resources on request
		nanosleep(&req.dur, &req.dur);

	}
	
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
	queue_count = 0;
	queue_space = n;

	// Setup semaphores
	sem_init(&mutex, 0, 1);
	sem_init(&sem_prod, 0, n);
	sem_init(&sem_cons, 0, 0);

	// Generate threads;
	pthread_t threads[n + 1];
	pthread_create(&threads[0], NULL, master, &queue);
	for(int i = 1; i < n + 1; i++) {
		pthread_create(&threads[i], NULL, slave, &queue);
	}

	// Run until user exits
	char cmd;
	do {
		scanf(" %c", &cmd);
	} while (cmd != 'x');

	// Alternate: run unill master thread terminates (currently never)
	//pthread_join(threads[0].thread, NULL);

	// Clean up and exit
	sem_destroy(&mutex);
	sem_destroy(&sem_prod);
	sem_destroy(&sem_cons);
	exit(0);

}