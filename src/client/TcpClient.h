#ifndef __TCP_CLINET_H__
#define __TCP_CLINET_H__
#include "NetClient.h"

namespace NetFwk
{
class CTcpClient : public CNetClient
{

public:
	CTcpClient();
	virtual ~CTcpClient();

	virtual bool connect();
	virtual int send(const char* buf, size_t len);
	virtual int recv(char* buf, size_t len);

protected:
	virtual int inSend(const char* buf, size_t len);
	virtual int inRecv(char* buf, size_t len);

private:
	int m_sockfd;
	size_t m_rLen;
	bool m_isConnect;
	char* m_buffer;
};

} //NetFwk
#endif //__TCP_CLINET_H__
