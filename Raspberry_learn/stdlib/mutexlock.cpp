#include <assert.h>
#include "mutexlock.h"
MutexLock::MutexLock():isLocking_(false)
{
    TINY_CHECK(!pthread_mutex_init(&mutex_, NULL));
}
MutexLock::~MutexLock()
{
    assert(!isLocking());
    TINY_CHECK(!pthread_mutex_destroy(&mutex_));
}
INT32 MutexLock::lock()
{
    INT32 res = pthread_mutex_lock(&mutex_);
    isLocking_ = true;
    return res;
}
INT32 MutexLock::trylock()
{
    INT32 res = pthread_mutex_trylock(&mutex_);
    isLocking_ = true;
    return res;
}
INT32 MutexLock::timedlock(const struct timespec* tsptr)
{
    INT32 res = pthread_mutex_timedlock(&mutex_, tsptr);
    isLocking_ = true;
    return res;
}
INT32 MutexLock::unlock()
{
    isLocking_ = false;
    return pthread_mutex_unlock(&mutex_);
}

bool MutexLock::isLocking() const
{
	return isLocking_;
}
pthread_mutex_t *MutexLock::getMutexPtr()
{
	return &mutex_;
}

VOID MutexLock::restoreMutexStatus()
{
	isLocking_ = true;
}

/*rw lock*/

RWLock::RWLock()
{
    pthread_rwlock_init(&rwlock, NULL);
}
RWLock::~RWLock()
{
    pthread_rwlock_destroy(&rwlock);
}
INT32 RWLock::rdlock()
{
    return pthread_rwlock_rdlock(&rwlock);
}
INT32 RWLock::wrlock()
{
    return pthread_rwlock_wrlock(&rwlock);
}
INT32 RWLock::unlock()
{
    return pthread_rwlock_unlock(&rwlock);
}
INT32 RWLock::tryrdlock()
{
    return pthread_rwlock_tryrdlock(&rwlock);
}
INT32 RWLock::trywrlock()
{
    return pthread_rwlock_trywrlock(&rwlock);
}
INT32 RWLock::rwlock_timedrdlock(const struct timespec* tsptr)
{
    return pthread_rwlock_timedrdlock(&rwlock, tsptr);
}
INT32 RWLock::rwlock_timedwrlock(const struct timespec* tsptr)
{
    return pthread_rwlock_timedwrlock(&rwlock, tsptr);
}

    //pthread_rwlock_t rwlock;