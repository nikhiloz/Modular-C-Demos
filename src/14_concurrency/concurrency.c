/*
 * Chapter 14 — Concurrency with POSIX Threads
 *
 * Threads share the same address space — that's both their power
 * and their danger.  Shared memory means fast communication, but
 * also means race conditions if you don't synchronise properly.
 *
 * Topics covered:
 *   1. Basic thread creation and joining
 *   2. Race conditions — why they happen, seeing one live
 *   3. Mutex — mutual exclusion to protect shared data
 *   4. Condition variables — waiting for a condition efficiently
 *   5. Producer-consumer pattern (mutex + condvar together)
 *   6. Thread-local storage (__thread keyword demo)
 *
 * Build: make 14_concurrency   (links with -lpthread)
 * Run:   ./bin/14_concurrency
 *
 * Try these:
 *   gcc -fsanitize=thread src/14_concurrency/concurrency.c -lpthread  # TSAN
 *   valgrind --tool=helgrind ./bin/14_concurrency   # race detector
 *   taskset -c 0 ./bin/14_concurrency   # pin to one core — race still exists!
 */

#define _DEFAULT_SOURCE     /* usleep() needs this with -std=c99 */
#include "../../include/common.h"
#include <pthread.h>
#include <unistd.h>

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Basic Thread Creation
 * ════════════════════════════════════════════════════════════════ */

/* pthread_create takes (pthread_t*, attr*, func(void*)->void*, void* arg).
 * Everything is void* because POSIX predates C generics — you cast.
 * The thread function receives one void* argument and returns void*.
 * The return value can be retrieved with pthread_join.              */

static void *basic_thread_func(void *arg)
{
    int id = *(int *)arg;       /* cast void* back to what we passed */
    printf("  Thread %d: started  (tid hint: %lu)\n", id, (unsigned long)pthread_self());
    usleep(50000);              /* simulate some work (50ms) */
    printf("  Thread %d: finished\n", id);
    return NULL;                /* return value retrievable by join */
}

static void demo_basic_thread(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Basic Thread Creation                   ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    /* Create two threads — they start executing immediately */
    pthread_create(&t1, NULL, basic_thread_func, &id1);
    pthread_create(&t2, NULL, basic_thread_func, &id2);

    /* Join blocks until the thread finishes.  Without join, main()
     * might exit and kill the threads before they complete. */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n  Both threads joined.  Order of output may vary —\n");
    printf("  that's the nature of concurrency: the OS schedules\n");
    printf("  threads independently.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Race Conditions — The Problem
 * ════════════════════════════════════════════════════════════════ */

/* Shared counter with NO protection — this WILL produce wrong results
 * because ++ is not atomic: it's load → increment → store.
 * Two threads can load the same value, both increment, both store,
 * and one increment is lost. */

static int unsafe_counter = 0;

static void *unsafe_increment(void *arg)
{
    (void)arg;
    for (int i = 0; i < 100000; i++)
        unsafe_counter++;       /* NOT atomic! read-modify-write race */
    return NULL;
}

static void demo_race_condition(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Race Condition (broken on purpose!)     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    unsafe_counter = 0;
    pthread_t threads[4];

    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, unsafe_increment, NULL);
    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    printf("  4 threads × 100,000 increments = %d  (expected 400,000)\n",
           unsafe_counter);
    if (unsafe_counter != 400000)
        printf("  *** RACE CONDITION: lost %d increments! ***\n",
               400000 - unsafe_counter);
    else
        printf("  (got lucky this time — race still exists)\n");

    printf("\n  What happens: counter++ is three steps:\n");
    printf("    1. LOAD  counter → register\n");
    printf("    2. ADD   1\n");
    printf("    3. STORE register → counter\n");
    printf("  Two threads can both LOAD the same value, both ADD 1,\n");
    printf("  both STORE — one increment is silently lost.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Mutex — Fixing the Race
 * ════════════════════════════════════════════════════════════════ */

static int safe_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *safe_increment(void *arg)
{
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&counter_mutex);     /* acquire exclusive access */
        safe_counter++;                          /* only one thread at a time */
        pthread_mutex_unlock(&counter_mutex);   /* release for others */
    }
    return NULL;
}

static void demo_mutex(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Mutex — Fixing the Race                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    safe_counter = 0;
    pthread_t threads[4];

    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, safe_increment, NULL);
    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    printf("  4 threads × 100,000 increments = %d  (expected 400,000)\n",
           safe_counter);
    printf("  Mutex guarantees: exactly one thread in the critical section.\n\n");

    printf("  Cost: mutex lock/unlock adds ~25ns overhead per operation.\n");
    printf("  For tight loops, consider batching or atomic operations.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Condition Variables
 * ════════════════════════════════════════════════════════════════ */

/* Condition variables let a thread WAIT for a condition to become true,
 * without busy-looping.  The thread releases the mutex while waiting
 * and re-acquires it when signalled.
 *
 * CRITICAL: Always use WHILE, not IF, around pthread_cond_wait!
 * Reason: "spurious wakeups" — the OS may wake your thread even
 * when nobody signalled.  The while loop re-checks the condition. */

#define BUFFER_SIZE 5
static int buffer[BUFFER_SIZE];
static int buf_count = 0;              /* items currently in buffer */
static pthread_mutex_t buf_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  not_empty  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  not_full   = PTHREAD_COND_INITIALIZER;

/* ════════════════════════════════════════════════════════════════
 *  Section 5: Producer-Consumer Pattern
 * ════════════════════════════════════════════════════════════════ */

/* This pattern needs BOTH mutex AND condvar:
 *   - Mutex protects the buffer (prevent corrupt reads/writes)
 *   - Condvar allows efficient waiting (no busy loop)
 * Without condvar, consumer would spin checking count — wasteful.
 * Without mutex, buffer access would race.                        */

static void *producer(void *arg)
{
    (void)arg;
    for (int i = 0; i < 8; i++) {
        pthread_mutex_lock(&buf_mutex);

        /* WHILE not IF: guard against spurious wakeups.
         * After being woken, re-check condition because another
         * thread might have grabbed the slot between signal and wake. */
        while (buf_count == BUFFER_SIZE)
            pthread_cond_wait(&not_full, &buf_mutex);

        buffer[buf_count++] = i;        /* produce item */
        printf("  [Producer] put %d  (count=%d/%d)\n", i, buf_count, BUFFER_SIZE);

        pthread_cond_signal(&not_empty);    /* wake one waiting consumer */
        pthread_mutex_unlock(&buf_mutex);
        usleep(30000);      /* simulate production time */
    }
    return NULL;
}

static void *consumer(void *arg)
{
    (void)arg;
    for (int i = 0; i < 8; i++) {
        pthread_mutex_lock(&buf_mutex);

        while (buf_count == 0)              /* nothing to consume? wait */
            pthread_cond_wait(&not_empty, &buf_mutex);

        int val = buffer[--buf_count];      /* consume item */
        printf("  [Consumer] got %d  (count=%d/%d)\n", val, buf_count, BUFFER_SIZE);

        pthread_cond_signal(&not_full);     /* wake producer if it was blocked */
        pthread_mutex_unlock(&buf_mutex);
        usleep(70000);      /* simulate processing time (slower than producer) */
    }
    return NULL;
}

static void demo_producer_consumer(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Producer-Consumer Pattern               ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    buf_count = 0;
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    printf("\n  Producer-Consumer completed.\n");
    printf("  Note: consumer is slower (70ms vs 30ms) so buffer fills up.\n");
    printf("  When full, producer WAITs on not_full condition variable.\n\n");

    printf("  Why WHILE not IF for cond_wait?\n");
    printf("    1. Spurious wakeups: OS may wake thread without signal\n");
    printf("    2. Stolen wake: another thread may act before you re-lock\n");
    printf("    3. Multiple waiters: signal wakes one, others must re-wait\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Thread-Local Storage
 * ════════════════════════════════════════════════════════════════ */

/* __thread (GCC) / _Thread_local (C11) gives each thread its OWN
 * copy of the variable.  Reads and writes don't need a mutex because
 * no other thread can see your copy.
 * Use case: per-thread errno, per-thread scratch buffers, per-thread RNG. */

static __thread int tls_counter = 0;    /* each thread has its own copy */

static void *tls_thread_func(void *arg)
{
    int id = *(int *)arg;

    /* Each thread increments its OWN tls_counter — no sharing */
    for (int i = 0; i < 1000; i++)
        tls_counter++;

    printf("  Thread %d: tls_counter = %d  (each thread sees 1000)\n",
           id, tls_counter);
    return NULL;
}

static void demo_thread_local(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Thread-Local Storage                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    pthread_t threads[3];
    int ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++)
        pthread_create(&threads[i], NULL, tls_thread_func, &ids[i]);
    for (int i = 0; i < 3; i++)
        pthread_join(threads[i], NULL);

    printf("  Main thread: tls_counter = %d  (untouched — it's per-thread)\n\n",
           tls_counter);

    printf("  What happens: __thread creates a SEPARATE copy of the\n");
    printf("  variable in each thread's storage.  No mutex needed because\n");
    printf("  no sharing occurs.  The main thread's copy stays at 0.\n");
    printf("  C11 spells it _Thread_local; GCC/Clang support __thread.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n=== C CONCURRENCY DEMO v%s ===\n", VERSION_STRING);

    demo_basic_thread();
    demo_race_condition();
    demo_mutex();
    demo_producer_consumer();
    demo_thread_local();

    DEMO_END();
    return 0;
}
