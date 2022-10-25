#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.h"
#include "thread.h"
#include "NetClient.h"

namespace NetFwk
{
class CTcpClient : public INetClient
{

public:
	CTcpClient();
	~CTcpClient();

	virtual bool init(const char* ip, unsigned int port);
	virtual bool attach(size_t recvLen, const INetClient::recvProc_t & proc);
	virtual bool connect();
	virtual int send(const char* buf, size_t len);
	virtual int recv(char* buf, size_t len);
	virtual bool close();

private:
	void recv_task(void* arg);

private:
	INetClient::recvProc_t m_proc;
	struct sockaddr_in m_addr;
	int m_sockfd;
	size_t m_rLen;
	bool m_isConnect;
	char* m_buffer;
	Infra::CThread* m_pThread; //服务器线程
};

CTcpClient::CTcpClient()
:INetClient()
,m_sockfd(-1)
,m_rLen(0)
,m_isConnect(false)
{
	memset(&m_addr, 0, sizeof(struct sockaddr_in));
}

CTcpClient::~CTcpClient()
{
	if (m_buffer != nullptr)
	{
		delete [] m_buffer;
	}
}

bool CTcpClient::init(const char* ip, unsigned int port)
{
	if (m_sockfd >= 0)
	{
		Infra::Error("netFwk", "client is already init!\n");
		return false;
	}

	m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
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

bool CTcpClient::connect()
{
	if (m_sockfd < 0)
	{
		Infra::Error("netFwk", "client need init!\n");
		return false;
	}

	if (m_isConnect)
	{
		Infra::Warning("netFwk", "client is already connect!\n");
		return false;
	}

	if (::connect(m_sockfd, (struct sockaddr*)&m_addr, sizeof(struct sockaddr_in)) != 0)
	{
		Infra::Warning("netFwk", "client connect %s:%d fail:%s!\n", (char*)inet_ntoa(m_addr.sin_addr), ntohs(m_addr.sin_port), strerror(errno));
		return false;
	}

	if (!m_proc.isEmpty())
	{
		m_pThread->run();
	}

	m_isConnect = true;
	return true;
}

int CTcpClient::send(const char* buf, size_t len)
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
		int sendlen = ::send(m_sockfd, p, len, 0);
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

int CTcpClient::recv(char* buf, size_t len)
{
	if (buf == NULL || len <= 0)
	{
		return -1;
	}

	if  (!m_proc.isEmpty())
	{
		Infra::Error("netFwk", "already regsiter recv callback function!\n");
		return -1;
	}

	int rlen = ::recv(m_sockfd, buf, len, 0);
	if (rlen > 0)
	{
		Infra::Debug("netFwk","recv:%s:%d len=%d\n", (char*)inet_ntoa(m_addr.sin_addr), ntohs(m_addr.sin_port), rlen);
	}

	return rlen;
}

bool CTcpClient::close()
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

bool CTcpClient::attach(size_t recvLen, const INetClient::recvProc_t & proc)
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
	

	if (!m_proc.isEmpty())
	{
		Infra::Warning("netFwk", "client is already set callback!\n");
		return false;
	}
	m_proc = proc;

	m_pThread = new Infra::CThread();
	m_pThread->attachProc(Infra::ThreadProc_t(&CTcpClient::recv_task, this));
	m_pThread->createTread();
	return true;
}


void CTcpClient::recv_task(void* arg)
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
	return nullptr;
}

} //NetFwk
