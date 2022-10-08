#ifndef __PROTOCL_H__
#define __PROTOCL_H__

#include <string>
#include "thread.h"
#include "timer.h"
#include "Session.h"

namespace NetFwk
{

class IProtocol
{
public:
	enum
	{
		Reply,
		NoReply,
		Logout,
	};

	typedef Infra::CFunc<void, IProtocol*, int> EventProc_t;
protected:
	IProtocol(ISession* session, size_t recvlen);
	virtual ~IProtocol();

public:
	virtual bool watchEvent(const EventProc_t& func);
	virtual int parse(char* buf, int len) = 0;
	virtual int send(const char* buf, int len);

protected:
	virtual void notify(int event);

private:
	void sessionTask(void* arg);
	void timerProc(unsigned long long arg);

protected:
	Infra::CThread m_thread;
	Infra::CTimer m_timer;
	EventProc_t m_event;
	ISession* m_session;
	const size_t m_recvLen;
	char* m_pBuffer;

};

} //NetFwk

#endif //__I_PROTOCL_H__
