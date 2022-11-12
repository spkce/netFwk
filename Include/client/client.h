#ifndef __CLINET_H__
#define __CLINET_H__
#include <stddef.h>
#include "thread.h"

namespace NetFwk
{
class INetClient
{
public:
	typedef enum Type_t
	{
		emTCPClient,
		emUDPClient,
	}Type_t;

	typedef Infra::CFunc<void, const char*, size_t> recvProc_t;

public:
	INetClient();
	virtual ~INetClient();

public:
	/**
	* @brief 客户端创建函数
	* @param type 客户端类型
	* @return 客户端接口
	**/
	static INetClient* create(Type_t type);

	/**
	* @brief 客户端初始化函数
	* @param ip 服务器ip地址
	* @param port 服务器端口
	* @return 成功：true；失败：false
	**/
	virtual bool init(const char* ip, unsigned int port) = 0;

	/**
	* @brief 注册接受回调函数
	* @param recvLen 接收缓冲区大小
	* @param proc 接收回调函数
	* @return 成功：true；失败：false
	**/
	virtual bool attach(size_t recvLen, const recvProc_t & proc) = 0;

	/**
	* @brief 连接服务器
	* @return 成功：true；失败：false
	**/
	virtual bool connect() = 0;

	/**
	* @brief 向服务器发送报文
	* @param buf 发送内容
	* @param len 发送内容长度
	* @return 剩余未发送的长度
	**/
	virtual int send(const char* buf, size_t len) = 0;

	/**
	* @brief 主动接收服务器回包
	* @param buf 接收缓冲区
	* @param len 接收缓冲区长度
	* @return 接收的长度
	**/
	virtual int recv(char* buf, size_t len) = 0;

	/**
	* @brief 关闭客户端
	* @return 成功：true；失败：false
	**/
	virtual bool close() = 0;

};


} //NetFwk

#endif //__CLINET_H__
