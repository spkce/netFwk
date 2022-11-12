#ifndef __NET_CLINET_H__
#define __NET_CLINET_H__

#include <netinet/in.h>
#include "thread.h"
#include "client.h"

namespace NetFwk
{

class CNetClient : public INetClient
{

public:
	CNetClient(Type_t type);
	virtual ~CNetClient();

	virtual bool init(const char* ip, unsigned int port);
	virtual bool attach(size_t recvLen, const INetClient::recvProc_t & proc);
	virtual bool close();

protected:
	virtual int inSend(const char* buf, size_t len) = 0;
	virtual int inRecv(char* buf, size_t len) = 0;
	void run();
	bool isAttach() const;

private:
	void recv_task(void* arg);

protected:
	struct sockaddr_in m_addr;

private:
	INetClient::recvProc_t m_proc;
	Infra::CThread* m_pThread;
	int m_sockfd;
	size_t m_rLen;
	bool m_isConnect;
	char* m_buffer;
	const Type_t m_type;
};

} //NetFwk

#endif //__NET_CLINET_H__
