
#include "stdio.h"
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include "Log.h"
#include "ctime.h"
#include "NetServer.h"
#include "Session.h"
#include "Protocol.h"



class CProtocol : public NetFwk::IProtocol
{

public:
	CProtocol(NetFwk::ISession* session, size_t recvLen);
	~CProtocol();
	virtual int parse(char* buf, int len);
private:
	/* data */
};

CProtocol::CProtocol(NetFwk::ISession* session, size_t recvLen)
:IProtocol(session, recvLen)
{
}

CProtocol::~CProtocol()
{
}

int CProtocol::parse(char* buf, int len)
{
	return IProtocol::NoReply;
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

/**
* @brief 构造函数
**/
CTermial::CTermial()
:NetFwk::ITerminal(5)
{

}

/**
* @brief 析构函数
**/
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


int main(int argc, char const *argv[])
{
	CTermial termial;
	termial.init(8000, -1);

	return 0;
}
