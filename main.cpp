
#include "stdio.h"
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include "Log.h"
#include "ctime.h"
#include "NetClient.h"
#include "NetServer.h"
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

int main(int argc, char const *argv[])
{
	CTermial termial;
	termial.init(8000, 5);

	CNetClient client;

	int i = 10;
	while (i-- > 0)
	{
		char buf[1024] = {0};
		client.send("hello world!", sizeof("hello world!"));
	
		sleep(1);
		client.recv(buf, sizeof(buf));
		printf("\033[0;35m""client recv:%s""\033[0m\n", buf);
		sleep(1);
	}

	return 0;
}
