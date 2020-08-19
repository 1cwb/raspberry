#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H
#include <pthread.h>
#include "common.h"

#include "condition.h"


class MutexLock
{
    friend class Condition;
public:
    MutexLock();
    ~MutexLock();
    INT32 lock();
    INT32 unlock();
    INT32 trylock();
    INT32 timedlock(const struct timespec* tsptr);
    bool isLocking() const;
    pthread_mutex_t *getMutexPtr();
private:
	VOID restoreMutexStatus();
	pthread_mutex_t mutex_;
	bool isLocking_;
};

class RWLock
{
public:
    RWLock();
    ~RWLock();
    INT32 rdlock();
    INT32 wrlock();
    INT32 unlock();
    INT32 tryrdlock();
    INT32 trywrlock();
    INT32 rwlock_timedrdlock(const struct timespec* tsptr);
    INT32 rwlock_timedwrlock(const struct timespec* tsptr);
private:
    pthread_rwlock_t rwlock;
};
#endif
