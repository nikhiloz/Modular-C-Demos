/**
 * @file concurrency.c
 * @brief C concurrency: pthreads, mutex, condition variables
 */
#include "../../include/common.h"
#include <pthread.h>
#include <unistd.h>

/* Shared data */
static int shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d started\n", id);
    usleep(100000);
    printf("Thread %d finished\n", id);
    return NULL;
}

void demo_basic_thread(void) {
    DEMO_SECTION("Basic Threads");
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    pthread_create(&t1, NULL, thread_func, &id1);
    pthread_create(&t2, NULL, thread_func, &id2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Both threads completed\n");
}

void* counter_thread(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&counter_mutex);
        shared_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

void demo_mutex(void) {
    DEMO_SECTION("Mutex Synchronization");
    shared_counter = 0;
    pthread_t threads[4];
    int ids[4] = {0, 1, 2, 3};
    
    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, counter_thread, &ids[i]);
    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);
    
    printf("4 threads x 1000 increments = %d (expected 4000)\n", shared_counter);
}

/* Producer-Consumer */
#define BUFFER_SIZE 5
static int buffer[BUFFER_SIZE];
static int count = 0;
static pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void* producer(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&buf_mutex);
        while (count == BUFFER_SIZE)
            pthread_cond_wait(&not_full, &buf_mutex);
        buffer[count++] = i;
        printf("Produced: %d (count=%d)\n", i, count);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&buf_mutex);
        usleep(50000);
    }
    return NULL;
}

void* consumer(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&buf_mutex);
        while (count == 0)
            pthread_cond_wait(&not_empty, &buf_mutex);
        int val = buffer[--count];
        printf("Consumed: %d (count=%d)\n", val, count);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&buf_mutex);
        usleep(100000);
    }
    return NULL;
}

void demo_producer_consumer(void) {
    DEMO_SECTION("Producer-Consumer Pattern");
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    printf("Producer-Consumer completed\n");
}

void demo_thread_local(void) {
    DEMO_SECTION("Thread-Local Storage");
    printf("__thread keyword provides thread-local storage\n");
    printf("Each thread gets its own copy of the variable\n");
}

int main(void) {
    printf("\n=== C CONCURRENCY DEMO v%s ===\n", VERSION_STRING);
    demo_basic_thread();
    demo_mutex();
    demo_producer_consumer();
    demo_thread_local();
    DEMO_END(); return 0;
}
