#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

int n, m;
int id_count = 0;
int cons_id_count = 0;
int queue_count, queue_space;

sem_t mutex, empty, full;

typedef struct {
	int id;
	struct timespec dur;
} request_t;

int rand_bound(int low, int high) {
	return rand() % (high - low + 1) + low;
}

long rand_nanos() {
	return (rand_bound(1, 1000) - 1) * 1000000l
		+ (rand_bound(1, 1000) - 1) * 1000l
		+ (rand_bound(1, 1000) - 1);
}

double secs(struct timespec t) {
	return (double)t.tv_sec + (double)t.tv_nsec / 1000000000.0;
}

/*void push(request_t req, request_t *queue) {

	sem_wait(&queue_space);

	sem_wait(&mutex);
	
	space--;
	queue[queue_count % n].id = req.id;
	queue[queue_count++ % n].dur = req.dur;

	sem_post(&mutex);

	sem_post(&avail_requests);

}*/

/*request_t pop(request_t *queue) {

	sem_wait(&mutex);

	sem_wait(&avail_requests);

	request_t result = queue[(queue_count + space++) % n];
	printf("result: %i %f\n", result.id, secs(result.dur));

	sem_post(&queue_space);

	sem_post(&mutex);

	return result;

}*/

void *master(void *arg) {

	request_t (*queue)[n] = (request_t (*)[n])arg;

	while(1) {

		printf("Producer: waiting for free space in the request queue.\n");
		
		sem_wait(&mutex);

		while(queue_space == 0) {
			sem_post(&mutex);
			sem_wait(&empty);
			sem_wait(&mutex);
		}

		int req_id = id_count++;
		struct timespec req_dur;
		req_dur.tv_sec = rand_bound(1, m) - 1;
		req_dur.tv_nsec = rand_nanos();
		printf("Producer: produced request id=%i of length %fs.\n", req_id, secs(req_dur));
	
		queue_space--;
		(*queue)[queue_count % n].id = req_id;
		(*queue)[queue_count++ % n].dur = req_dur;

		sem_post(&mutex);

		sem_post(&full);

		// Sleep for between 0 and 999999999 nanoseconds
		struct timespec sleep_dur;
		sleep_dur.tv_nsec = rand_nanos();
		printf("Producer: sleeping for %fs.\n", (double)(sleep_dur.tv_nsec/1000000000.0));
		nanosleep(&sleep_dur, &sleep_dur);

	}

	pthread_exit(NULL);

}

void *slave(void *arg) {

	request_t (*queue)[n] = (request_t (*)[n])arg;

	int id = cons_id_count++;

	while(1) {

		printf("Consumer id=%i: Waiting for request to be available.\n", id);
		
		sem_wait(&mutex);

		while(queue_space == n) {
			sem_post(&mutex);
			sem_wait(&full);
			sem_wait(&mutex);
		}

		request_t req = (*queue)[(queue_count + queue_space++) % n];
		printf("Consumer id=%i: Handling request id=%i, length=%fs.\n", id, req.id, secs(req.dur));

		sem_post(&mutex);

		sem_post(&empty);

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
	sem_init(&empty, 0, n);
	sem_init(&full, 0, 0);

	// Generate threads;
	pthread_t threads[n + 1];
	pthread_create(&threads[0], NULL, master, &queue);

	for(int i = 1; i < n + 1; i++) {
		pthread_create(&threads[i], NULL, slave, &queue);
	}

	// Hold until user input (temporary)
	char *cmd;
	scanf(" %c", cmd);
	//pthread_join(threads[0].thread, NULL);

	sem_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);

	exit(0);

}