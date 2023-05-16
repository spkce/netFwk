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
:m_sockfd(-1)
,m_isConnect(false)
,m_rLen(0)
,m_buffer(nullptr)
,m_type(type)
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

bool CNetClient::init(const char* ip, unsigned int port, int timeout)
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

	if (timeout > 0)
	{
		struct timeval _timeout = {0,0};
		_timeout.tv_sec = timeout;
		if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO,(char*)&_timeout,sizeof(struct timeval)) == -1)
		{
			Infra::Error("netFwk","setsockopt error : %s\n", strerror(errno));
			return false;
		}
		
		if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO,(char*)&_timeout,sizeof(struct timeval)) == -1)
		{
			Infra::Error("netFwk","setsockopt error : %s\n", strerror(errno));
			return false;
		}
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

int CNetClient::send(const char* buf, size_t len)
{
	if (m_sockfd < 0)
	{
		return m_sockfd;
	}

	if (buf == NULL || len <= 0)
	{
		return -1;
	}

	char* p = (char*)buf;

	Infra::Debug("netFwk", "send len : %lu -> %s:%d\n", len, (char*)inet_ntoa(m_addr.sin_addr), ntohs(m_addr.sin_port));

	while (len > 0)
	{
		int sendlen = inSend(p, len);
		if (sendlen > 0 )
		{
			len -= sendlen;
			p += sendlen;
		}
		else if (errno == EINTR)
		{
			continue;
		}
		else
		{
			Infra::Error("netFwk","send err : %s\n", strerror(errno));
			close();
			return -1;
		}
	}

	return len;
}

int CNetClient::recv(char* buf, size_t len)
{
	if (buf == NULL || len <= 0)
	{
		return -1;
	}

	if (isAttach())
	{
		Infra::Error("netFwk", "already regsiter recv callback function!\n");
		return -1;
	}

	int rlen = inRecv(buf, len);
	if (rlen > 0)
	{
		Infra::Debug("netFwk","recv:%s:%d len=%d\n", (char*)inet_ntoa(m_addr.sin_addr), ntohs(m_addr.sin_port), rlen);
	}

	return rlen;
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
		m_proc(m_buffer, rlen);
	}	
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
