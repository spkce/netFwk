#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__
#include "NetClient.h"

namespace NetFwk
{

class CUdpClient : public CNetClient
{
public:
	CUdpClient();
	virtual ~CUdpClient();

	virtual bool connect();
	virtual int send(const char* buf, size_t len);
	virtual int recv(char* buf, size_t len);
	//virtual bool close();
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

} //NetFwk
#endif //__UDP_CLIENT_H__
