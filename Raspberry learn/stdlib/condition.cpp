#include "condition.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include "common.h"

Condition::Condition(MutexLock &mutex) : mutex_(mutex)
{
	TINY_CHECK(!pthread_cond_init(&cond_, NULL));
}
Condition::~Condition()
{
	TINY_CHECK(!pthread_cond_destroy(&cond_));
}
INT32 Condition::wait()
{
	assert(mutex_.isLocking());
	TINY_CHECK(!pthread_cond_wait(&cond_, mutex_.getMutexPtr()));
	mutex_.restoreMutexStatus();
    return 0;
}
INT32 Condition::wait(ULONG nms)
{
    INT32 res = -1;
    struct timespec outtime;
    if(clock_gettime(CLOCK_REALTIME, &outtime))
    {
	DBG("get time fail");
    	return res;	
    }
    outtime.tv_sec += (INT32)(nms / 1000);
    outtime.tv_nsec += (nms % 1000) * 1000000;
    if(outtime.tv_nsec > 1000000000)
    {
        outtime.tv_sec += 1;
	outtime.tv_nsec -= 1000000000UL;
    }
    assert(mutex_.isLocking());
    res = pthread_cond_timedwait(&cond_, mutex_.getMutexPtr(), &outtime);
    mutex_.restoreMutexStatus();
    return res;
}
VOID Condition::notify()
{
	TINY_CHECK(!pthread_cond_signal(&cond_));
}
VOID Condition::notifyAll()
{
	TINY_CHECK(!pthread_cond_broadcast(&cond_));
}

