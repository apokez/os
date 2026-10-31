#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORKERS 3
#define ITERATIONS 100000

static long counter = 0;
static long worker_ids[WORKERS];
static sem_t counter_lock;
static sem_t start_gate;

static void *worker(void *arg) {
    long id = *(long *)arg;
    sem_wait(&start_gate);
    for (int i = 0; i < ITERATIONS; ++i) {
        sem_wait(&counter_lock);
        ++counter;
        sem_post(&counter_lock);
    }
    printf("worker %ld selesai\n", id);
    return NULL;
}

int main(void) {
    pthread_t threads[WORKERS];
    if (sem_init(&counter_lock, 0, 1) != 0 || sem_init(&start_gate, 0, 0) != 0) {
        perror("sem_init");
        return EXIT_FAILURE;
    }

    for (long i = 0; i < WORKERS; ++i) {
        worker_ids[i] = i;
        int rc = pthread_create(&threads[i], NULL, worker, &worker_ids[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < WORKERS; ++i) sem_post(&start_gate);
    for (int i = 0; i < WORKERS; ++i) pthread_join(threads[i], NULL);

    printf("counter aktual : %ld\n", counter);
    printf("counter harapan: %d\n", WORKERS * ITERATIONS);
    sem_destroy(&start_gate);
    sem_destroy(&counter_lock);
    return EXIT_SUCCESS;
}
