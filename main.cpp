
#include "stdio.h"
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include "Log.h"
#include "ctime.h"
#include "client.h"
#include "TcpServer.h"
#include "Session.h"
#include "Protocol.h"



class CProtocol : public NetFwk::IProtocol
{

public:
	CProtocol(NetFwk::ISession* session, size_t recvLen);
	~CProtocol();
	virtual int parse(const char* buf, int len);
private:
	bool m_isLogin;
};

CProtocol::CProtocol(NetFwk::ISession* session, size_t recvLen)
:IProtocol(session, recvLen)
,m_isLogin(false)
{
}

CProtocol::~CProtocol()
{
}

int CProtocol::parse(const char* buf, int len)
{
	if (!m_isLogin)
	{
		login();
	}

	printf("\033[0;35m""server recv:%s""\033[0m\n", buf);
	send(buf, len);
	return IProtocol::OK;
}

class CTermial : public NetFwk::ITerminal
{
public:

	CTermial();
	virtual ~CTermial();
	virtual NetFwk::IProtocol* createProtocol(NetFwk::ISession* session, size_t recvLen);
	virtual int getState();

private:

};

CTermial::CTermial()
:NetFwk::ITerminal(5, 1024)
{

}

CTermial::~CTermial()
{

}


NetFwk::IProtocol* CTermial::createProtocol(NetFwk::ISession* session, size_t recvLen)
{
	return new CProtocol(session, recvLen);
}


int CTermial::getState()
{
	return 0;
}

class CNetClient
{
public:
	CNetClient();
	virtual ~CNetClient();

	int send(const char* buf, unsigned int len);
	int recv(char* buf, unsigned int len);
private:
	NetFwk::INetClient* m_client;
};

CNetClient::CNetClient()
{
	m_client = NetFwk::INetClient::create(NetFwk::INetClient::emTCPClient);
	m_client->init("127.0.0.1", 8000);
	m_client->connect();
}

CNetClient::~CNetClient()
{
	delete m_client;
}

int CNetClient::send(const char* buf, unsigned int len)
{
	return m_client->send(buf, len);
}

int CNetClient::recv(char* buf, unsigned int len)
{
	return m_client->recv(buf, len);
}


struct stVcpHeader
{
	unsigned int ident;
	unsigned short reserve;
	unsigned short fsc;
	unsigned int length;
};

short checkSum(const unsigned char* buffer, int size)
{
	unsigned long cksum = 0;
	unsigned short * pbuf = (unsigned short *)buffer;
	for (int i = 0; i < size / 2; i++)
	{
		cksum += htons(pbuf[i]);
	}
	
	if( size % 2 == 1) 
	{ 
		cksum += (unsigned short)(buffer[size - 1] << 8);
	}
	
	while (cksum >> 16) 
	{
		cksum = (cksum >> 16) + (cksum & 0xffff); 
	}
	
	return (unsigned short)(~cksum);
}

int main(int argc, char const *argv[])
{
	//CTermial termial;
	//termial.init(8000, 5);

	CNetClient client;

	//unsigned char buf[1024] = {0}; 
	//int i = 10;
	//while (i-- > 0)
	//{
	char buf[1024] = {0};


	unsigned char* p = (unsigned char*)buf;
	struct stVcpHeader * phdr = (struct stVcpHeader *)p;

	const char* str1 = "{\"id\":1,\"group\":\"AArch32 VFP/NEON\", \"name\":\"Q1\",\"msgID\":107, \"sessionID\":140214441877680}";
	const int len1 = strlen(str1);
	unsigned char* pBody = p + sizeof(struct stVcpHeader);

	phdr->ident = htonl(0x7E766370);
	phdr->length = htonl(len1 + sizeof(struct stVcpHeader));
	phdr->fsc = htons(checkSum((unsigned char*)str1, len1));
	memcpy(pBody, str1, len1);


	//client.send((char*)p, len1 + sizeof(struct stVcpHeader));

	p += len1 + sizeof(struct stVcpHeader);

	const char* str2 = "{\"id\":1,\"group\":\"AArch32 VFP/NEON\", \"name\":\"Q2\",\"msgID\":107, \"sessionID\":140214441877680}";
	const int len2 = strlen(str2);
	phdr = (struct stVcpHeader *)p;
	pBody = p + sizeof(struct stVcpHeader);
	
	phdr->ident = htonl(0x7E766370);
	phdr->length = htonl(len2 + sizeof(struct stVcpHeader));
	phdr->fsc = htons(checkSum((unsigned char*)str2, len2));
	memcpy(pBody, str2, len2);

	//client.send((char*)p, len2 + sizeof(struct stVcpHeader));

	p += len2 + sizeof(struct stVcpHeader);

	const char* str3 = "{\"id\":1,\"group\":\"AArch32 VFP/NEON\", \"name\":\"Q3\",\"msgID\":107, \"sessionID\":140214441877680}";
	const int len3 = strlen(str3);
	phdr = (struct stVcpHeader *)p;
	pBody = p + sizeof(struct stVcpHeader);

	phdr->ident = htonl(0x7E766370);
	phdr->length = htonl(len3 + sizeof(struct stVcpHeader));
	phdr->fsc = htons(checkSum((unsigned char*)str3, len3));
	memcpy(pBody, str3, len3);
	
	//client.send((char*)buf, len3 + len1 + len2 + 3*(sizeof(struct stVcpHeader)));

	client.send((char*)buf, 102+51);
	client.send((char*)buf + 102+51, 102+51);
	p += len3 + sizeof(struct stVcpHeader);

		//client.send("hello world!", sizeof("hello world!"));
	//
		//sleep(1);
		//client.recv(buf, sizeof(buf));
		//printf("\033[0;35m""client recv:%s""\033[0m\n", buf);
		
		while(1);
//	}

	return 0;
}
