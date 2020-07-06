#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>
#include "common.h"
#include "mutexlock.h"

typedef void* (*threadFunc)(void*);
class Thread
{
public:
	Thread();
	virtual ~Thread();
	VOID start();
	VOID join();
	
	bool setThreadFunc(threadFunc func, VOID* data);	
	pthread_t getThreadID() const;
private:
	threadFunc thFunc;
    VOID* param;
	pthread_t threadId_;
	bool isRunning_;
};


#endif
