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
	bool Cpthread_equal(const pthread_t tid);
	static pthread_t Cpthread_self();
	static INT32 Cpthread_join(pthread_t tid, VOID** rval_ptr);
    static INT32 Cpthread_cancel(pthread_t tid);
	static VOID Cpthread_exit(VOID* rval_ptr);
    static INT32 Cpthread_detach(pthread_t tid);
private:
	threadFunc thFunc;
    VOID* param;
	pthread_t threadId_;
	bool isRunning_;
};


#endif
