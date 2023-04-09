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
{

}

CTcpClient::~CTcpClient()
{
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

int CTcpClient::inSend(const char* buf, size_t len)
{
	return ::send(m_sockfd, buf, len, 0);
}

int CTcpClient::inRecv(char* buf, size_t len)
{
	return ::recv(m_sockfd, buf, len, 0);
}

/*
int CTcpClient::inSend(const char* buf, size_t len)
{
	const char* p = buf;
	size_t length = len;
	while (length > 0)
	{
		int n = ::send(m_sockfd, p, length, 0);
		if (n < 0)
		{
			return n;
		}
		else if (n == 0)
		{
			continue;
		}
		p += n;
		length -= n;
	}
	
	return len;
}

int CTcpClient::inRecv(char* buf, size_t len)
{
	char* p = buf;
	size_t length = len;
	while (length > 0)
	{
		int n = ::recv(m_sockfd, p, length, 0)
		if (n < 0)
		{
			return n;
		}
		else if (n == 0)
		{
			return len - length
		}
		p += n;
		length -= n;
	}

	return len;
}

*/
} //NetFwk
