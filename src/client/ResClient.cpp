#include "ResClient.h"

namespace NetFwk
{

CResClient::CResClient()
:m_id(0)
{
}

CResClient::CResClient(CResClient && o)
:m_id(0)
{
	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	std::swap(m_id, o.m_id);
	m_buffer = std::move(o.m_buffer);
}

CResClient::CResClient(int id, Infra::CByteBuffer && buffer)
:m_id(id)
{
	m_buffer = buffer;
}

CResClient::~CResClient() 
{

}

int CResClient::id() const
{
	return m_id;
}

void CResClient::setId(int id) 
{
	m_id = id;
}

size_t CResClient::size()
{
	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	return m_buffer.size();
}

bool CResClient::input(const unsigned char* buf, size_t len)
{
	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	if (m_buffer.size())
	{
		m_buffer.append(buf, len);
		return true;
	}
	return false;
}

const unsigned char* CResClient::get()
{
	Infra::CGuard<Infra::CMutex> guard(m_mutex);
	return m_buffer.getBuffer();
}

CResClient& CResClient::operator=(CResClient && req)
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
