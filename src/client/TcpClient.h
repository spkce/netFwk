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

	virtual bool connect() override;
	virtual int send(const char* buf, size_t len) override;
	virtual int recv(char* buf, size_t len) override;

protected:
	virtual int inSend(const char* buf, size_t len) override;
	virtual int inRecv(char* buf, size_t len) override;

};

} //NetFwk
#endif //__TCP_CLINET_H__
