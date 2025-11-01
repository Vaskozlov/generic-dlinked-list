#include "dlinked_list.h"
#include "rwlock.h"

#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>

struct rwlock lock;
atomic_uint writers_count;
atomic_uint readers_count;
atomic_uint reading_iterations;
atomic_uint writing_iterations;

void *reader(void *)
{
    while (true) {
        rwlock_rdlock(&lock);

        atomic_fetch_add(&readers_count, 1);

        if (atomic_load(&writers_count) > 0) {
            printf("Writing in progress!\n");
        }

        atomic_fetch_sub(&readers_count, 1);
        atomic_fetch_add(&reading_iterations, 1);

        rwlock_rdunlock(&lock);
    }

    return NULL;
}

void *writer(void *)
{
    while (true) {
        rwlock_wrlock(&lock);

        if (atomic_fetch_add(&writers_count, 1) > 0) {
            printf("More than one writers\n");
        }

        if (atomic_load(&readers_count) > 0) {
            printf("Reading in progress!\n");
        }

        atomic_fetch_sub(&writers_count, 1);
        atomic_fetch_add(&writing_iterations, 1);

        rwlock_wrunlock(&lock);
    }

    return NULL;
}

void signal_handler(int sig)
{
#ifdef __cplusplus
    using namespace std;
#endif

    switch (sig) {
    case SIGINT:
        printf(
            "r: %u, w: %u\n", atomic_load(&reading_iterations), atomic_load(&writing_iterations));
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
    pthread_t readers[4];

    for (size_t i = 0; i < 4; ++i) {
        pthread_create(&readers[i], NULL, reader, NULL);
    }

    pthread_t writers[4];

    for (size_t i = 0; i < 4; ++i) {
        pthread_create(&writers[i], NULL, writer, NULL);
    }

    pthread_join(writers[0], NULL);
}
