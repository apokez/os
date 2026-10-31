#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 4
#define ITEMS 8

static int buffer[CAPACITY];
static int in = 0;
static int out = 0;
static sem_t empty_slots;
static sem_t full_slots;
static sem_t buffer_lock;

static void *producer(void *unused) {
    (void)unused;
    for (int value = 1; value <= ITEMS; ++value) {
        sem_wait(&empty_slots);
        sem_wait(&buffer_lock);
        buffer[in] = value;
        printf("produksi %d pada slot %d\n", value, in);
        in = (in + 1) % CAPACITY;
        sem_post(&buffer_lock);
        sem_post(&full_slots);
    }
    return NULL;
}

static void *consumer(void *unused) {
    (void)unused;
    for (int i = 0; i < ITEMS; ++i) {
        sem_wait(&full_slots);
        sem_wait(&buffer_lock);
        int value = buffer[out];
        printf("konsumsi %d dari slot %d\n", value, out);
        out = (out + 1) % CAPACITY;
        sem_post(&buffer_lock);
        sem_post(&empty_slots);
    }
    return NULL;
}

int main(void) {
    pthread_t producer_thread, consumer_thread;
    if (sem_init(&empty_slots, 0, CAPACITY) != 0 ||
        sem_init(&full_slots, 0, 0) != 0 ||
        sem_init(&buffer_lock, 0, 1) != 0) {
        perror("sem_init");
        return EXIT_FAILURE;
    }
    int rc = pthread_create(&producer_thread, NULL, producer, NULL);
    if (rc != 0) {
        fprintf(stderr, "pthread_create (producer): %s\n", strerror(rc));
        return EXIT_FAILURE;
    }
    rc = pthread_create(&consumer_thread, NULL, consumer, NULL);
    if (rc != 0) {
        fprintf(stderr, "pthread_create (consumer): %s\n", strerror(rc));
        pthread_cancel(producer_thread);
        pthread_join(producer_thread, NULL);
        return EXIT_FAILURE;
    }
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    sem_destroy(&buffer_lock);
    sem_destroy(&full_slots);
    sem_destroy(&empty_slots);
    puts("selesai: seluruh item diproses");
    return EXIT_SUCCESS;
}
