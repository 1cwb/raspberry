#include <assert.h>
#include "thread.h"
Thread::Thread() : thFunc(NULL), param(NULL), threadId_(0), isRunning_(false)
{
	
}
Thread::~Thread()
{
	if(isRunning_)
	{
	    isRunning_ = false;
	    TINY_CHECK(!pthread_detach(threadId_));	
	}
}

VOID Thread::start()
{
	TINY_CHECK(!pthread_create(&threadId_, NULL, thFunc,param));
	isRunning_ = true;
}
VOID Thread::join()
{
	assert(isRunning_);
	TINY_CHECK(!pthread_join(threadId_, NULL));
	isRunning_ = false;
}

pthread_t Thread::getThreadID() const 
{
	return threadId_;
}
bool Thread::setThreadFunc(threadFunc func, VOID* data)
{
    if(func == NULL)
    {
        return false;
    }
    thFunc = func;
    param = data;
    return true;
}

pthread_t Thread::Cpthread_self()
{
	return pthread_self();
}
bool Thread::Cpthread_equal(const pthread_t tid)
{
	if(pthread_equal(threadId_, tid) != 0)
	{
		return true;
	}
	return false;
}
INT32 Thread::Cpthread_join(pthread_t thread, VOID** rval_ptr)
{
    return pthread_join(thread, rval_ptr);
}

INT32 Thread::Cpthread_cancel(pthread_t tid)
{
	return pthread_cancel(tid);
}
VOID Thread::Cpthread_exit(VOID* rval_ptr)
{
    pthread_exit(rval_ptr);
}
INT32 Thread::Cpthread_detach(pthread_t tid)
{
	return pthread_detach(tid);
}