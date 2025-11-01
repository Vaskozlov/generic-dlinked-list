#ifndef RWLOCK_H
#define RWLOCK_H

#if defined(__cplusplus)
#    include <atomic>
#else
#    include <stdatomic.h>
#endif

#if defined(__cplusplus)
typedef std::atomic_uint_least64_t rwlock_atomic_t;
#else
typedef atomic_uint_least64_t rwlock_atomic_t;
#endif

enum RwLockWritingStatus
{
    RwLockWritingStatus_NONE = 0,
    RwLockWritingStatus_READING = 2,
    RwLockWritingStatus_WRITING = 1,
};

struct rwlock
{
    rwlock_atomic_t status;
};

#if defined(__cplusplus)
extern "C"
{
#endif

    inline void rwlock_init(struct rwlock *lock)
    {
        lock->status = RwLockWritingStatus_NONE;
    }

    inline void rwlock_wrlock(struct rwlock *lock)
    {
#if defined(__cplusplus)
        using namespace std;
#endif

        uint64_t expected;

        do {
            expected = RwLockWritingStatus_NONE;
        } while (!atomic_compare_exchange_weak_explicit(
            &lock->status,
            &expected,
            RwLockWritingStatus_WRITING,
            memory_order_acq_rel,
            memory_order_relaxed));
    }

    inline void rwlock_wrunlock(struct rwlock *lock)
    {
#if defined(__cplusplus)
        using namespace std;
#endif

        uint64_t expected = atomic_load_explicit(&lock->status, memory_order_relaxed);
        const uint64_t mask = ~((uint64_t)RwLockWritingStatus_WRITING);

        while (!atomic_compare_exchange_weak_explicit(
            &lock->status,
            &expected,
            expected & mask,
            memory_order_acq_rel,
            memory_order_relaxed)) {
        }
    }

    inline void rwlock_rdlock(struct rwlock *lock)
    {
#if defined(__cplusplus)
        using namespace std;
#endif

        const uint64_t mask = ~((uint64_t)RwLockWritingStatus_WRITING);
        uint64_t expected = atomic_load_explicit(&lock->status, memory_order_relaxed);

        do {
            expected &= mask;
        } while (!atomic_compare_exchange_weak_explicit(
            &lock->status,
            &expected,
            expected + RwLockWritingStatus_READING,
            memory_order_acq_rel,
            memory_order_relaxed));
    }

    inline void rwlock_rdunlock(struct rwlock *lock)
    {
#if defined(__cplusplus)
        using namespace std;
#endif

        uint64_t expected = atomic_load_explicit(&lock->status, memory_order_relaxed);

        while (!atomic_compare_exchange_weak_explicit(
            &lock->status,
            &expected,
            expected - RwLockWritingStatus_READING,
            memory_order_acq_rel,
            memory_order_relaxed)) {
        }
    }

#if defined(__cplusplus)
}
#endif

#endif /* RWLOCK_H */
