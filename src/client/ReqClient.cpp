#include "ReqClient.h"

namespace NetFwk
{

CReqClient::CReqClient()
:m_id(0)
{
}

CReqClient::~CReqClient() 
{

}

int CReqClient::id() const
{
	return m_id;
}

void CReqClient::setId(int id) 
{
	m_id = id;
}

size_t CReqClient::size()
{
	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	return m_buffer.size();
}

bool CReqClient::input(const unsigned char* buf, size_t len)
{
	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	if (m_buffer.size())
	{
		m_buffer.append(buf, len);
		return true;
	}
	return false;
}

const unsigned char* CReqClient::get()
{
	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	return m_buffer.getBuffer();
}

CReqClient& CReqClient::operator=(CReqClient && req)
{
	m_id = req.m_id;
	Infra::CByteBuffer temp;

	req.m_mutex.lock();
	temp = std::move(req.m_buffer);
	req.m_id = 0;
	req.m_mutex.unlock();

	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	m_buffer = std::move(temp);
	return *this;
}

} //NetFwk
