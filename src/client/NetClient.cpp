#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.h"
#include "TcpClient.h"
#include "UdpClient.h"
#include "NetClient.h"

namespace NetFwk
{

CNetClient::CNetClient(Type_t type)
:m_type(type)
{
	memset(&m_addr, 0, sizeof(struct sockaddr_in));
}

CNetClient::~CNetClient()
{
	if (m_buffer != nullptr)
	{
		delete [] m_buffer;
	}
}

bool CNetClient::init(const char* ip, unsigned int port)
{
	if (m_sockfd >= 0)
	{
		Infra::Error("netFwk", "client is already init!\n");
		return false;
	}

	m_sockfd = ::socket(AF_INET, m_type == INetClient::emTCPClient ? SOCK_STREAM : SOCK_DGRAM, 0);
	if (m_sockfd < 0)
	{
		Infra::Error("netFwk", "open socket fail!\n");
		return false;
	}

	unsigned int nIpAddr = inet_addr(ip);
	if(nIpAddr == INADDR_NONE) 
	{
		Infra::Error("netFwk", "the ip address format is incorrect!\n");
		return false;
	}

	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = nIpAddr;
	m_addr.sin_port = htons(port);
	return true;
}

bool CNetClient::attach(size_t recvLen, const INetClient::recvProc_t & proc)
{
	if (m_sockfd < 0)
	{
		Infra::Error("netFwk", "client is not init!\n");
		return false;
	}

	if (m_isConnect)
	{
		Infra::Warning("netFwk", "client is already connect!\n");
		return false;
	}

	if (recvLen > 0)
	{
		if (m_buffer != nullptr)
		{
			delete [] m_buffer;
		}
		m_buffer = new char[recvLen];
		m_rLen = recvLen;
	}
	else
	{
		Infra::Error("netFwk", "recv buffer must greater than 0!\n");
		return false;
	}
	

	if (isAttach())
	{
		Infra::Warning("netFwk", "client is already set callback!\n");
		return false;
	}
	m_proc = proc;

	m_pThread = new Infra::CThread();
	m_pThread->attachProc(Infra::ThreadProc_t(&CNetClient::recv_task, this));
	m_pThread->createTread();
	return true;
}

bool CNetClient::close()
{
	if (m_sockfd < 0)
	{
		Infra::Error("netFwk", "client is not init!\n");
		return false;
	}

	if (!m_proc.isEmpty())
	{
		m_pThread->stop();
	}

	m_proc.unbind();

	::close(m_sockfd);

	return true;
}

void CNetClient::run()
{
	if (m_pThread != nullptr)
	{
		m_pThread->run();
	}
}

bool CNetClient::isAttach() const
{
	return !m_proc.isEmpty();
}

void CNetClient::recv_task(void* arg)
{
	int rlen = ::recv(m_sockfd, m_buffer, m_rLen, 0);
	if (rlen > 0)
	{
		Infra::Debug("netFwk","recv:%s:%d len=%d\n", (char*)inet_ntoa(m_addr.sin_addr), ntohs(m_addr.sin_port), rlen);
	}

	m_proc(m_buffer, rlen);
}

INetClient::INetClient()
{
}

INetClient::~INetClient()
{
}


INetClient* INetClient::create(Type_t type)
{
	if (type == emTCPClient)
	{
		return new CTcpClient;
	}
	else if (type == emUDPClient)
	{
		return new CUdpClient; 
	}
	return nullptr;
}

} //NetFwk
