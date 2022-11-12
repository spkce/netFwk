#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.h"
#include "thread.h"
#include "NetClient.h"
#include "UdpClient.h"

namespace NetFwk
{

CUdpClient::CUdpClient()
:CNetClient(INetClient::emUDPClient)
{

}

CUdpClient::~CUdpClient()
{

}

bool CUdpClient::connect()
{
	return true;
}

int CUdpClient::send(const char* buf, size_t len)
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
		int sendlen = ::sendto(m_sockfd, buf, len, 0, (struct sockaddr*)&m_addr, sizeof(struct sockaddr_in));
		if (sendlen > 0)
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

int CUdpClient::recv(char* buf, size_t len)
{
	//recvform(m_sockfd, buf, len, 0, nullptr, nullptr);
	if (buf == NULL || len <= 0)
	{
		return -1;
	}

	if  (!m_proc.isEmpty())
	{
		Infra::Error("netFwk", "already regsiter recv callback function!\n");
		return -1;
	}

	int rlen = ::recvfrom(m_sockfd, buf, len, 0, nullptr, nullptr);
	if (rlen > 0)
	{
		Infra::Debug("netFwk","recv:%s:%d len=%d\n", (char*)inet_ntoa(m_addr.sin_addr), ntohs(m_addr.sin_port), rlen);
	}

	return rlen;
}

} //NetFwk
