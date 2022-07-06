#ifndef __I_PROTOCL_H__
#define __I_PROTOCL_H__

#include <string>

namespace NetServer {
	class ISession;
}

namespace Terminal {
	class ITerminal;
}

namespace Screen
{

/**
* @brief 协议接口类
**/
class IProtocl
{	
public:
	/**
	* @brief 支持的协议类型
	**/
	typedef  enum protocl_t
	{
		//xxxx
	}protocl_t;
protected:
	/**
	* @brief 构造函数，设定protected权限，不可实例，可继承
	**/
	IProtocl();
	/**
	* @brief 析构函数，设定protected权限，不可实例，可继承
	**/
	virtual ~IProtocl();
public:
	/**
	* @brief 创建协议实例
	* @param type 协议类型
	* @param termial 终端
	* @return 协议接口
	**/
	static IProtocl * createInstance(protocl_t type, Terminal::ITerminal* termial);
	/**
	* @brief 取消协议实例
	* @param protocl 议接指针
	**/
	static void cancelInstance(IProtocl * protocl);

	/**
	* @brief 协议解析函数,具体协议类必须实现
	* @param session 会话话指针
	* @param buf 接收到的内容
	* @param len 接收到的内容长度
	* @return 成功/失败
	**/
	virtual bool probe(NetServer::ISession* session, char* buf, int len) = 0;

	/**
	* @brief 协议解析函数,具体协议类必须实现
	* @param session 会话话指针
	* @param buf 接收到的内容
	* @param len 接收到的内容长度
	* @return 成功/失败
	**/
	virtual bool parse(NetServer::ISession* session, char* buf, int len) = 0;
	/**
	* @brief 封装消息,具体协议类必须实现
	* @param msgID 消息ID
	* @param buf 接收到的内容
	* @param len 接收到的内容长度
	* @return 成功/失败
	**/
	virtual bool notify(NetServer::ISession* session, char* buf, int len) = 0;
	/**
	* @brief 获取版本信息
	* @param ver 版本信息
	**/
	void getVersion(std::string & ver);

protected:
	std::string m_version;
};





} //Screen

#endif //__I_PROTOCL_H__
