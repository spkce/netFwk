#include "ctime.h"
#include "NetClient.h"
#include "ProtoClient.h"

namespace NetFwk
{

IProtoClient::IProtoClient(INetClient::Type_t type)
:m_timeout(-1)
,m_type(type)
,m_client(nullptr)
{
}

IProtoClient::~IProtoClient()
{
	if (m_client)
	{
		m_client->close();
		delete m_client;
		m_client = nullptr;
	}
}


bool IProtoClient::init(const char* ip, unsigned int port, int timeout)
{
	if (m_client)
	{
		return false;
	}

	m_client = NetFwk::INetClient::create(m_type);
	m_client->init(ip, port, timeout);

	m_timeout = timeout;
	m_client->attach(4096, INetClient::recvProc_t(&IProtoClient::recv_proc, this));
	return m_client->connect();
}

bool IProtoClient::close()
{
	if (!m_client)
	{
		return false;
	}
	
	m_client->close();
	delete m_client;
	m_client = nullptr;

	m_rwlock.wLock();
	for (auto it = m_mapRequest.begin(); it != m_mapRequest.end(); it++)
	{
		CReqClient* p = it->second;
		p->setId(0);
	}

	m_mapRequest.clear();
	m_rwlock.unLock();

	return true;
}

bool IProtoClient::send(const unsigned char* buf, size_t len, CReqClient * req)
{
	if (m_client)
	{
		return false;
	}

	if ((int)len != m_client->send((const char*)buf, len))
	{
		return false;
	}
	
	if (!req)
	{
		return true;
	}

	appendResopne(req);

	if (m_timeout >= 0)
	{
		unsigned long long ctime = Infra::CTime::getProcessTimeSecond();
		unsigned long long etime = ctime + m_timeout;
		
		while (ctime > etime)
		{
			if (!req->id())
			{
				break;
			}

			if (req->size())
			{
				deleteResopne(req);
				return true;
			}
			
			Infra::CTime::delay_ms(300);
			ctime = Infra::CTime::getProcessTimeSecond();
		}
		
		deleteResopne(req);
		return false;
	}
	else
	{
		while (1)
		{
			if (!req->id())
			{
				deleteResopne(req);
				return false;
			}

			if (req->size())
			{
				deleteResopne(req);
				return true;
			}
			
			Infra::CTime::delay_ms(300);
		}
	}
}

bool IProtoClient::dispense(CReqClient* p)
{
	int id = p->id();
	m_rwlock.rLock();
	auto it = m_mapRequest.find(id);
	if (it == m_mapRequest.end())
	{
		m_rwlock.unLock();
		return false;
	}
	CReqClient * payload = it->second;
	m_rwlock.unLock();

	*payload = std::move(*p);
	return false;
}

bool IProtoClient::appendResopne(CReqClient* p)
{
	int id = p->id();
	m_rwlock.wLock();
	if (m_mapRequest.find(id) != m_mapRequest.end())
	{
		m_rwlock.unLock();
		return false;
	}

	m_mapRequest[id] = p;
	m_rwlock.unLock();
	return true;
}

bool IProtoClient::deleteResopne(CReqClient* p)
{
	int id = p->id();
	m_rwlock.wLock();
	auto it = m_mapRequest.find(id);
	if ( it != m_mapRequest.end())
	{
		m_rwlock.unLock();
		return false;
	}
	m_mapRequest.erase(it);
	m_rwlock.unLock();
	return true;
}

void IProtoClient::recv_proc(const char* buf, size_t len)
{
	CReqClient req;
	if (parser((const unsigned char*)buf, len, &req))
	{
		dispense(&req);
	}
}

} //NetFwk
