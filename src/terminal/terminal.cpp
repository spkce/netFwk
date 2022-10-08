#include "Log.h"
#include "terminal.h"
#include "Session.h"
#include "NetServer.h"

namespace NetFwk
{

ITerminal::ITerminal(int max)
:m_vecProtocol(max)
,m_pServer(nullptr)
,m_maxSession(max)
,m_port(0)
{
	
}

ITerminal::~ITerminal()
{
	m_pServer->stop();

	//delete m_pServer();
	//delete m_vecProtocol
}

bool ITerminal::init(unsigned int port, int timeout)
{
	if (m_port == 0)
	{
		return false;
	}

	m_pServer = INetServer::create(m_port, INetServer::emTCPServer);
	m_pServer->attach(INetServer::ServerProc_t(&ITerminal::serverTask, this));
	m_pServer->start(m_maxSession);

	return true;
}

bool ITerminal::serverTask(int sockfd, struct sockaddr_in* addr)
{
	if (m_vecProtocol.size() < m_maxSession)
	{
		Infra::Warning("netFwk", "the number of session reaches its maximun !\n");
		return false;
	}

	ISession* pSession = ISession::create(sockfd, addr, 15);
	if (pSession == nullptr)
	{
		Infra::Warning("netFwk", "Session create fail!\n");
		return false;
	}

	IProtocol* p = createProtocol(pSession, 1024);
	p->watchEvent(IProtocol::EventProc_t(&ITerminal::eventTask, this));
	if (p == nullptr)
	{
		Infra::Warning("netFwk", "Procotol create fail!\n");
		return false;
	}

	m_vecProtocol.push_back(p);
	return true;
}

void ITerminal::eventTask(IProtocol* p, int event)
{
	if (event == IProtocol::Logout)
	{
		//for m_vecProtocol
	}
}

} // NetFwk
