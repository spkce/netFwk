#ifndef __NET_INTERFACE_H__
#define __NET_INTERFACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <list>

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

class CNetIF
{
private:

public:
	CNetIF();
	~CNetIF();
};

CNetIF::CNetIF(/* args */)
{
}

CNetIF::~CNetIF()
{
}

#endif //__NET_INTERFACE_H__