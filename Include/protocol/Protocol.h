#ifndef __PROTOCL_H__
#define __PROTOCL_H__

#include <string>

namespace NetFwk
{

class IProtocol
{	
protected:
	/**
	* @brief 构造函数，设定protected权限，不可实例，可继承
	**/
	IProtocol();
	/**
	* @brief 析构函数，设定protected权限，不可实例，可继承
	**/
	virtual ~IProtocol();
public:

	/**
	* @brief 协议解析函数,具体协议类必须实现
	* @param session 会话话指针
	* @param buf 接收到的内容
	* @param len 接收到的内容长度
	* @return 成功/失败
	**/
	virtual bool parse(char* buf, int len) = 0;
	/**
	* @brief 封装消息,具体协议类必须实现
	* @param msgID 消息ID
	* @param buf 接收到的内容
	* @param len 接收到的内容长度
	* @return 成功/失败
	**/
	virtual bool notify(char* buf, int len) = 0;
	/**
	* @brief 获取版本信息
	* @param ver 版本信息
	**/
	virtual void getVersion(std::string & ver);

protected:
	std::string m_version;
};





} //NetFwk

#endif //__I_PROTOCL_H__
