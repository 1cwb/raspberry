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
    VOID lock();
    VOID unlock();
    
    bool isLocking() const;
    pthread_mutex_t *getMutexPtr();
private:
	VOID restoreMutexStatus();
	pthread_mutex_t mutex_;
	bool isLocking_;
};

#endif
