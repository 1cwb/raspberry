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
	bool CpthreadEqual(const pthread_t tid);
	static pthread_t CpthreadSelf();
	static INT32 CpthreadJoin(pthread_t tid, VOID** rval_ptr);
    static INT32 CpthreadCancel(pthread_t tid);
	static VOID CpthreadExit(VOID* rval_ptr);
    static INT32 CpthreadDetach(pthread_t tid);
	static INT32 CpthreadOnce(pthread_once_t *initflag, VOID (*initfn)(void));
private:
	threadFunc thFunc;
    VOID* param;
	pthread_t threadId_;
	bool isRunning_;
};

class ThreadKey
{
public:
    ThreadKey(VOID (*destr_function) (VOID*) = destrFunction);
    ~ThreadKey();
    INT32 pthreadSetspecific(const VOID* pointer);
    VOID* pthreadGetspecific();
    static VOID destrFunction(VOID* value);
private:
    pthread_key_t key;
    
};

#endif
