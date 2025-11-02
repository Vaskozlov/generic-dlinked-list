#ifndef RWLOCK_H
#define RWLOCK_H

#if defined(__cplusplus)
#    include <atomic>
#else
#    include <stdatomic.h>
#endif

#if defined(__cplusplus)
typedef std::atomic_uint rwlock_atomic_uint;
#else
typedef atomic_uint rwlock_atomic_uint;
#endif

enum RwLockWritingStatus
{
    RwLockWritingStatus_NONE = 0,
    RwLockWritingStatus_READING = 2,
    RwLockWritingStatus_WRITING = 1,
};

struct rwlock
{
    rwlock_atomic_uint value;
};

#if defined(__cplusplus)
extern "C"
{
#endif

    inline void rwlock_init(struct rwlock *lock)
    {
#if defined(__cplusplus)
        using namespace std;
#endif
        atomic_store_explicit(&lock->value, RwLockWritingStatus_NONE, memory_order_relaxed);
    }

    inline void rwlock_wrlock(struct rwlock *lock)
    {
#if defined(__cplusplus)
        using namespace std;
#endif
        unsigned expected;

        do {
            expected = RwLockWritingStatus_NONE;
        } while (!atomic_compare_exchange_weak_explicit(
            &lock->value,
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

        atomic_fetch_sub(&lock->value, RwLockWritingStatus_WRITING);
    }

    inline void rwlock_rdlock(struct rwlock *lock)
    {
#if defined(__cplusplus)
        using namespace std;
#endif

        const unsigned mask = ~((unsigned)RwLockWritingStatus_WRITING);
        unsigned expected = atomic_load_explicit(&lock->value, memory_order_relaxed);

        do {
            expected &= mask;
        } while (!atomic_compare_exchange_weak_explicit(
            &lock->value,
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

        atomic_fetch_sub_explicit(&lock->value, RwLockWritingStatus_READING, memory_order_acq_rel);
    }

#if defined(__cplusplus)
    }
#endif

#endif /* RWLOCK_H */
