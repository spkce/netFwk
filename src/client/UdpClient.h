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

	virtual bool connect() override;

protected:
	virtual int inSend(const char* buf, size_t len) override;
	virtual int inRecv(char* buf, size_t len) override;

};

} //NetFwk
#endif //__UDP_CLIENT_H__
