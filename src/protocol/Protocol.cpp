#include <string.h>
#include <map>
#include "thread.h"
#include "protocol.h"

namespace NetFwk
{

IProtocol::IProtocol(ISession* session, size_t recvLen)
:m_timer()
,m_session(session)
,m_recvLen(recvLen)
{

	m_pBuffer = new char [recvLen];

	m_thread.attachProc(Infra::ThreadProc_t(&IProtocol::sessionTask, this));

	if (session->getTimeout() > 0)
	{
		m_timer.setTimerAttr(Infra::CTimer::TimerProc_t(&IProtocol::timerProc, this), 3000);
	}
}

IProtocol::~IProtocol()
{
	if (m_session->getTimeout() > 0 && m_timer.isRun())
	{
		m_timer.stop();
	}

	m_thread.detachProc(Infra::ThreadProc_t(&IProtocol::sessionTask, this));

	delete [] m_pBuffer;

	m_session->destroy();
}

bool IProtocol::watchEvent(const EventProc_t& func)
{
	if (m_event.isEmpty())
	{
		m_event = func;
		return true;
	}
	return false;
}

int IProtocol::send(const char* buf, int len)
{
	if (m_session->getState() == ISession::emStateLogin)
	{
		return m_session->send(buf, len);
	}
	return -1;
}

void IProtocol::notify(int event)
{
	if (!m_event.isEmpty())
	{
		m_event(this, event);
	}
}

void IProtocol::sessionTask(void* arg)
{
	if (m_session->getState() == ISession::emStateLogin)
	{
		memset(m_pBuffer, 0, m_recvLen);
		int len = m_session->recv(m_pBuffer, m_recvLen);
		if (len <= 0 )
		{
			return;
		}

		parse(m_pBuffer, len);
	}
}

void IProtocol::timerProc(unsigned long long arg)
{
	if (m_session->isTimeout())
	{
		//Infra::Debug("netFwk", "Session:%s:%d time out\n", (char*)inet_ntoa(m_addr.sin_addr), ntohs(m_addr.sin_port));
		m_session->logout();
		m_session->close();
		notify(Logout);
	}
}

} //NetFwk
