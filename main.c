#include "dlinked_list.h"
#include "rwlock.h"

#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>

#define READING_THREADS 4
#define WRITING_THREADS 4

struct rwlock lock;
atomic_uint writers_count;
atomic_uint readers_count;
atomic_uint reading_iterations;
atomic_uint writing_iterations;

struct timespec start;

void *reader(void *)
{
    while (true) {
        rwlock_rdlock(&lock);

        atomic_fetch_add_explicit(&readers_count, 1, memory_order_relaxed);

        if (atomic_load_explicit(&writers_count, memory_order_relaxed) > 0) {
            printf("Writing in progress!\n");
        }

        atomic_fetch_sub_explicit(&readers_count, 1, memory_order_relaxed);
        atomic_fetch_add_explicit(&reading_iterations, 1, memory_order_relaxed);

        rwlock_rdunlock(&lock);
    }

    return NULL;
}

void *writer(void *)
{
    while (true) {
        rwlock_wrlock(&lock);

        if (atomic_fetch_add_explicit(&writers_count, 1, memory_order_relaxed) > 0) {
            printf("More than one writers\n");
        }

        if (atomic_load_explicit(&readers_count, memory_order_relaxed) > 0) {
            printf("Reading in progress!\n");
        }

        atomic_fetch_sub_explicit(&writers_count, 1, memory_order_relaxed);
        atomic_fetch_add_explicit(&writing_iterations, 1, memory_order_relaxed);

        rwlock_wrunlock(&lock);
    }

    return NULL;
}

void signal_handler(int sig)
{
#ifdef __cplusplus
    using namespace std;
#endif

    struct timespec end;

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_ms =
        (double)((end.tv_sec - start.tv_sec) * 1000LL + (end.tv_nsec - start.tv_nsec) / 1000000LL)
        / 1000.0;

    const unsigned reads = atomic_load(&reading_iterations);
    const unsigned writes = atomic_load(&writing_iterations);

    switch (sig) {
    case SIGINT:
        printf(
            "r: %u, w: %u, r: %.0f op/sec, w: %.0f op/sec\n",
            reads,
            writes,
            (double)reads / elapsed_ms,
            (double)writes / elapsed_ms);
        break;
    default:
        exit(0);
    }
}

int main()
{
    // struct List list = list_init();

    // list_push_back(&list, int, 0);
    // list_push_back(&list, int, 1);
    // list_push_back(&list, int, 2);
    // list_push_back(&list, int, 6);

    // list_for_each(&list, int, node_name, ptr_name, { printf("%d\n", *ptr_name); });

    // list_erase(&list, list.head->next->next);

    // list_for_each(&list, int, node_name, ptr_name, { printf("%d\n", *ptr_name); });

    // struct List second_list = list_init();
    // list_copy(&list, &second_list, int);

    // list_destroy(&list);
    // list_destroy(&second_list);

    signal(SIGINT, signal_handler);

    rwlock_init(&lock);
    pthread_t readers[READING_THREADS];
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < READING_THREADS; ++i) {
        pthread_create(&readers[i], NULL, reader, NULL);
    }

    pthread_t writers[WRITING_THREADS];

    for (size_t i = 0; i < WRITING_THREADS; ++i) {
        pthread_create(&writers[i], NULL, writer, NULL);
    }

    pthread_join(writers[0], NULL);
}
