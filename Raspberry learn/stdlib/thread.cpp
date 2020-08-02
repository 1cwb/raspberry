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

