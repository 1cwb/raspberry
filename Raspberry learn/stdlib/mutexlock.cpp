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
VOID MutexLock::lock()
{
    TINY_CHECK(!pthread_mutex_lock(&mutex_));
    isLocking_ = true;	
}
VOID MutexLock::unlock()
{
	isLocking_ = false;
	TINY_CHECK(!pthread_mutex_unlock(&mutex_));
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


