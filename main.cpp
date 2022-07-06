
#include "stdio.h"
#include <unistd.h>
#include "stdio.h"
#include <string.h>
#include <algorithm>
#include "NetServer.h"
#include "Session.h"
#include "IProtocl.h"
#include "Log.h"
#include "ctime.h"



#if 0
class CTermial : public NetFwk::ITerminal
{
public:
	/**
	* @brief 构造函数
	**/
	CTermial();

	/**
	* @brief 析构函数
	**/
	virtual ~CTermial();

	/**
	* @brief 初始化
	* @return 成功：true；失败：false
	**/
	virtual bool init();

	/**
	* @brief 获取状态
	* @return 状态
	**/
	virtual int getState();

	/**
	* @brief session 连接
	* @param session 会话指针
	* @param type 协议类型
	* @return 成功：true；失败：false
	**/
	virtual bool connect(NetFwk::ISession* session, int type);

	/**
	* @brief session 断开连接
	* @param session 会话指针
	* @param type 协议类型
	* @return 成功：true；失败：false
	**/
	virtual bool disconnet(NetFwk::ISession* session, int type);
	/**
	* @brief 获取版本信息
	* @param ver 版本信息
	* @return 成功/失败
	**/
	virtual bool getVersion(std::string & ver);
	/**
	* @brief 消息推送
	* @param buf 消息内容
	* @param len 消息长度
	* @return 成功：true；失败：false
	**/
	virtual bool notify(char* buf, int len);

	/**
	* @brief 服务器回调函数
	* @param sockfd 套接字句柄
	* @param addr 对端套接字地址
	* @return 成功：true；失败：false
	**/
	bool serverTask(int sockfd, struct sockaddr_in* addr);
	
	/**
	* @brief 会话回调函数
	* @param session 会话指针
	* @param buf 消息内容
	* @param len 消息长度
	**/
	void sessionTask(NetFwk::ISession* session, char* buf, int len);


private:
#define PORT_MAIN 7877
#define MAX_SESSION 5
	const int m_maxSession;
	const int m_portMain;
	NetFwk::INetServer* m_pServ;
	IProtocl* m_protocl;
	NetFwk::ISession* m_session;

};

/**
* @brief 构造函数
**/
CTermial::CTermial()
:ITerminal(ITerminal::emTerminalScree)
,m_maxSession(MAX_SESSION)
,m_portMain(PORT_MAIN)
,m_pServ(NULL)
,m_session(NULL)
{

	m_protocl = IProtocl::createInstance(0, this);
}

/**
* @brief 析构函数
**/
CTermial::~CTermial()
{
	m_pServGps->stop();
	m_pServ->stop();
	IProtocl::cancelInstance(m_protocl);

}

/**
* @brief 初始化
* @return 成功：true；失败：false
**/
bool CTermial::init()
{
	m_pServ = INetServer::create(m_portMain, INetServer::emTCPServer);
	m_pServ->attach(INetServer::ServerProc_t(&CTermial::serverTask, this));
	m_pServ->start(m_maxSession);

	return true;
}

/**
* @brief 获取状态
* @return 状态
**/
int CTermial::getState()
{
	//只判断主连接
	if (m_session != NULL && m_session->getState() == ISession::emStateLogin)
	{
		return ITerminal::emInline;
	}

	return ITerminal::emOffline;
}

/**
* @brief session 连接
* @param session 会话指针
* @param type 协议类型
* @return 成功：true；失败：false
**/
bool CTermial::connect(ISession* session, int type)
{
	//
	if (session == NULL)
	{
		Error("NetTerminal", "input session null !!!\n");
		return false;
	}

	m_session = session;
	
	return false;
}

/**
* @brief session 断开连接
* @param session 会话指针
* @param type 协议类型
* @return 成功：true；失败：false
**/
bool CTermial::disconnet(ISession* session, int type)
{
	CSessionManager::instance()->cancelSession(m_session);
	return false;
}

/**
* @brief 获取版本信息
* @param ver 版本信息
* @return 成功/失败
**/
bool CTermial::getVersion(std::string & ver)
{
	if (CSessionManager::instance()->getState(m_session) == ISession::emStateLogin)
	{
		m_protocl->getVersion(ver);
		return true;
	}
	return false;
}

/**
* @brief 消息推送
* @param buf 消息内容
* @param len 消息长度
* @return 成功：true；失败：false
**/
bool CTermial::notify(char* buf, int len)
{
	return m_protocl->notify(m_session, buf, len);
}

/**
* @brief 服务器回调函数
* @param sockfd 套接字句柄
* @param addr 对端套接字地址
**/
bool CTermial::serverTask(int sockfd, struct sockaddr_in* addr)
{
	if (CSessionManager::instance()->getState(m_session) == ISession::emStateLogin)
	{
		Warning("NetTerminal", "Gps Session was registered !\n");
		return false;
	}

	ISession* pSession = CSessionManager::instance()->createSession(sockfd, addr, 15);
	if (pSession == NULL)
	{
		return false;
	}

	if (!pSession->bind(ISession::SessionProc_t(&CTermial::sessionTask, this)))
	{
		CSessionManager::instance()->cancelSession(pSession);
	}
	return true;
}

/**
* @brief 会话回调函数
* @param session 会话指针
* @param buf 消息内容
* @param len 消息长度
**/
void CTermial::sessionTask(NetServer::ISession* session, char* buf, int len)
{
	if (session == NULL || buf == NULL || len <= 0)
	{
		return ;
	}
	
	m_protocl->parse(session, buf, len);
}
#endif
int main(int argc, char const *argv[])
{

	return 0;
}
