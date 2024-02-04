#ifndef __REQ_CLIENT_H__
#define __REQ_CLIENT_H__
#include "thread.h"
#include "ByteBuffer.h"

namespace NetFwk
{

//template<>

class CResClient
{
public:
	CResClient();
	CResClient(CResClient && o);
	CResClient(int id, Infra::CByteBuffer && buffer);
	virtual ~CResClient();
	
	int id() const;
	void setId(int id);
	size_t size();
	bool input(const unsigned char* buf, size_t len);
	const unsigned char* get();
	
	CResClient& operator=(CResClient && req);

private:
	Infra::CMutex m_mutex;
	Infra::CByteBuffer m_buffer;
	int m_id;

};

} //NetFwk
#endif //__REQ_CLIENT_H__
