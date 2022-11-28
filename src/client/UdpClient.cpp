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
	m_isConnect = true;
	return true;
}

int CUdpClient::inSend(const char* buf, size_t len)
{
	return ::sendto(m_sockfd, buf, len, 0, (struct sockaddr*)&m_addr, sizeof(struct sockaddr_in));
}

int CUdpClient::inRecv(char* buf, size_t len)
{
	return ::recvfrom(m_sockfd, buf, len, 0, nullptr, nullptr);
}

} //NetFwk
