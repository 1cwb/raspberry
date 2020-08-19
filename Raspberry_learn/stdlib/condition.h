#ifndef CONDITION_H
#define CONDITION_H
#include <pthread.h>
#include "common.h"
#include "mutexlock.h"
class MutexLock;

class Condition
{
public:
	Condition(MutexLock &mutex);
	~Condition();
	INT32 wait();
        INT32 wait(ULONG nms);
	VOID notify();
	VOID notifyAll();
private:
	Condition(const MutexLock &mutex);
	pthread_cond_t cond_;
	MutexLock &mutex_;
};



#endif
