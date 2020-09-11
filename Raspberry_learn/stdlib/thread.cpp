#include <assert.h>
#include "thread.h"
#include "clog.h"
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
	thFunc = NULL;
	param = NULL;
	threadId_ = 0;
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

pthread_t Thread::CpthreadSelf()
{
	return pthread_self();
}
bool Thread::CpthreadEqual(const pthread_t tid)
{
	if(pthread_equal(threadId_, tid) != 0)
	{
		return true;
	}
	return false;
}
INT32 Thread::CpthreadJoin(pthread_t thread, VOID** rval_ptr)
{
    return pthread_join(thread, rval_ptr);
}

INT32 Thread::CpthreadCancel(pthread_t tid)
{
	return pthread_cancel(tid);
}
VOID Thread::CpthreadExit(VOID* rval_ptr)
{
    pthread_exit(rval_ptr);
}
INT32 Thread::CpthreadDetach(pthread_t tid)
{
	return pthread_detach(tid);
}

/*thread_key*/
ThreadKey::ThreadKey(VOID (*destr_function) (VOID*))
{
    pthread_key_create(&key ,destr_function);
}
ThreadKey::~ThreadKey()
{
    pthread_key_delete(key);
}
INT32 ThreadKey::pthreadSetspecific(const VOID* pointer)
{
    return pthread_setspecific(key ,pointer);
}
VOID* ThreadKey::pthreadGetspecific()
{
    return pthread_getspecific(key);
}
VOID ThreadKey::destrFunction(VOID* value)
{
    if(value != NULL)
    {
        free(value);
        printf("free value!\n");
    }
}
