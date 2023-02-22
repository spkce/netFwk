#ifndef __NET_IF_H__
#define __NET_IF_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <list>

struct in_mac
{
	unsigned char buf[6];
};

struct netif_name
{
	char buf[16];
};

typedef int (*INPUT)(char* buf, int len);
struct netif_info
{
	int ifindex;
	struct in_mac mac;
	struct netif_name name;
	struct in_addr ip;
	struct in_addr netmask;
	struct in_addr broadcast;
	INPUT input;
};

class INetIF
{
public:
	INetIF();
	virtual ~INetIF();
public:
	static char* macFormat(struct in_mac &mac, char format = ':');
	virtual int init() = 0;
	virtual int update() = 0;
	virtual int getNetCardNum() = 0;
	//virtual int send(char* buf, int len) = 0;
	virtual int attach(INPUT fun) = 0;
	virtual int attach(const char* ifName, INPUT fun) = 0;
	virtual int detach(INPUT fun) = 0;
	virtual int detach(const char* ifName, INPUT fun) = 0;
};

class CNetIF: public INetIF
{
public:
	CNetIF();
	virtual ~CNetIF();
public:
	int init();
	int update();
	int getNetCardNum();
	int send(char* buf, int len);
	int attach(INPUT fun);
	int attach(const char* ifName, INPUT fun);
	int detach(INPUT fun);
	int detach(const char* ifName, INPUT fun);
	void Dump();
	int recv(char* buf);
	int startRecv(const char* ifName = NULL);
private:
	int update(int socket);
	int send(char* buf, int len, int ifindex);
	
private:
	int m_socket;
	int m_netifNum;
	std::list<struct netif_info> m_listNetif;
};

#endif //__NET_IF_H__