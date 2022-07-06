#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "TFuncation.h"

namespace Infra
{

class CThreadCore;

class CPoolThread 
{
	friend class CThreadCore;
public:
	typedef TFuncation1<void, void *> ThreadProc_t;
public:
	CPoolThread();
	virtual ~CPoolThread();

	bool run();
	bool stop(bool isBlock);
	bool attach(const ThreadProc_t & proc);
	bool detach(const ThreadProc_t & proc);
private:
	CThreadCore* m_threadCore;
	ThreadProc_t m_proc;
};

} //Infra

#endif //__THREAD_POOL_H__
