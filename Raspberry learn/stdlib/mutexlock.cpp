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

}
RWLock::~RWLock()
{

}
INT32 RWLock::rdlock()
{

}
INT32 RWLock::wrlock()
{

}
INT32 RWLock::unlock()
{

}
INT32 RWLock::tryrdlock()
{

}
INT32 RWLock::trywrlock()
{

}
INT32 RWLock::rwlock_timedrdlock(const struct timespec* tsptr)
{

}
INT32 RWLock::rwlock_timedwrlock(const struct timespec* tsptr)
{

}

    //pthread_rwlock_t rwlock;