#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <string>
#include "Session.h"
#include "Protocol.h"

namespace NetFwk
{
class INetServer;

class ITerminal
{
public:
	enum
	{
		emInline,
		emOffline,
	};

protected:
	ITerminal(int max);
	virtual ~ITerminal();
	
public:
	virtual bool init(unsigned int port, int timeout);

	virtual int getState() = 0;

	virtual IProtocol* createProtocol(ISession* session, size_t recvLen) = 0;

	bool serverTask(int sockfd, struct sockaddr_in* addr);
	void eventTask(IProtocol* p, int event);

protected:
	Infra::CTimer m_timer;
	std::vector<IProtocol*> m_vecProtocol;
	INetServer* m_pServer;
	const unsigned int m_maxSession;
	unsigned int m_port;
};

} // NetFwk
#endif //__TERMINAL_H__
