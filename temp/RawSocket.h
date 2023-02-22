
#ifndef __RAW_SOCKET_H__
#define __RAW_SOCKET_H__

#include "NetIf.h"


class CRawSocket
{
public:
	CRawSocket();
	virtual ~CRawSocket();
public:
	int arpRequest();
	int ip_send();
	void input(char* buf, int len);
	void output(char* buf, int len);
private:
	void arp_output();
	void ip_input(char* buf, int len);
	void arp_input(char* buf, int len);
	//struct ether_arp* fillArpPacket(const struct in_addr dst_ip, const struct in_mac dst_mac, const struct in_addr src_ip, const struct in_addr src_mac);
};

#endif //__RAW_SOCKET_H__
