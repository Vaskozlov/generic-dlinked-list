#include "rwlock.h"

extern inline void rwlock_init(struct rwlock *lock);

extern inline void rwlock_wrlock(struct rwlock *lock);

extern inline void rwlock_wrunlock(struct rwlock *lock);

extern inline void rwlock_rdlock(struct rwlock *lock);

extern inline void rwlock_rdunlock(struct rwlock *lock);
