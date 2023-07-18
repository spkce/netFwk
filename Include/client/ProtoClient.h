#ifndef __PROTO_CLIENT_H__
#define __PROTO_CLIENT_H__

#include <map>
#include "thread.h"
#include "ctime.h"
#include "client.h"
#include "ReqClient.h"
namespace NetFwk
{

class IProtoClient
{
protected:
	IProtoClient(INetClient::Type_t type);

	virtual ~IProtoClient();

public:
	virtual bool init(const char* ip, unsigned int port, int timeout = -1);
	virtual bool close();
	virtual int parser(const unsigned char* buf, size_t len, CReqClient * req) = 0;
	
	virtual bool send(const unsigned char* buf, size_t len, CReqClient * req = nullptr);
	virtual bool dispense(CReqClient* p);

private:
	bool appendResopne(CReqClient* p);
	bool deleteResopne(CReqClient* p);
	void recv_proc(const char* buf, size_t len);

private:
	int m_timeout;
	const INetClient::Type_t m_type;
	NetFwk::INetClient* m_client;
	Infra::CRwlock m_rwlock;
	std::map<int, CReqClient*> m_mapRequest;
};



} //NetFwk

#endif //__PROTO_CLIENT_H__
