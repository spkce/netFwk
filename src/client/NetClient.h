#ifndef __NET_CLINET_H__
#define __NET_CLINET_H__

#include <netinet/in.h>
#include "thread.h"
#include "client.h"

namespace NetFwk
{

class CNetClient : public INetClient
{

protected:
	CNetClient(Type_t type);
	virtual ~CNetClient();

public:
	virtual bool init(const char* ip, unsigned int port, int timeout) override;
	virtual int send(const char* buf, size_t len) override;
	virtual int recv(char* buf, size_t len) override;
	virtual bool attach(size_t recvLen, const INetClient::recvProc_t & proc) override;
	virtual bool close() override;

protected:
	virtual int inSend(const char* buf, size_t len) = 0;
	virtual int inRecv(char* buf, size_t len) = 0;
	bool isAttach() const;
	void run();

private:
	void recv_task(void* arg);

protected:
	struct sockaddr_in m_addr;
	int m_sockfd;
	bool m_isConnect;

private:
	INetClient::recvProc_t m_proc;
	Infra::CThread* m_pThread;
	size_t m_rLen;
	char* m_buffer;
	const Type_t m_type;
};

} //NetFwk

#endif //__NET_CLINET_H__
