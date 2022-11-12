#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.h"
#include "thread.h"
#include "TcpClient.h"

namespace NetFwk
{

CTcpClient::CTcpClient()
:CNetClient(INetClient::emTCPClient)
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

	if (isAttach())
	{
		run();
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

	if (isAttach())
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

} //NetFwk
