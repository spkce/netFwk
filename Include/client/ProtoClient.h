#ifndef __PROTO_CLIENT_H__
#define __PROTO_CLIENT_H__

#include <list>
#include <map>
#include "thread.h"
#include "ctime.h"
#include "client.h"
#include "ResClient.h"

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
	virtual int parser(const unsigned char* buf, size_t len, std::list<CResClient> & list) = 0;
	
	virtual bool send(const unsigned char* buf, size_t len, CResClient * req = nullptr);

private:
	bool dispense(CResClient* p);
	bool appendResopne(CResClient* p);
	bool deleteResopne(CResClient* p);
	void recv_proc(const char* buf, size_t len);

private:
	int m_timeout;
	const INetClient::Type_t m_type;
	NetFwk::INetClient* m_client;
	Infra::CRwlock m_rwlock;
	std::map<int, CResClient*> m_mapRequest;
};



} //NetFwk

#endif //__PROTO_CLIENT_H__
