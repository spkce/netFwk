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
#include <string>
#include "cfunc.h"
#include "thread.h"

class CMac
{
	//"a1:b2:c3:e4:f5:f6"
public:
	CMac();
	CMac(unsigned char mac[6]);
	CMac(const char* str, char split = ':');
	CMac(const CMac & mac);
	virtual ~CMac();
	CMac& operator=(const CMac& mac);
	bool operator==(const CMac& mac) const;
	const char* format(char split = ':');
	unsigned char m_mac[6];
};

struct NetIfInfo
{
	uint32_t index;
	uint32_t ip;
	uint32_t netmask
	uint32_t broadcast;
	CMac mac;
	std::string name;
};

class CNetInterface;

class CNetCapture
{
public:
	typedef Infra::CFunc<void, const char* buf, int len> input_t;

protected:
	CNetCapture();
	virtual ~CNetCapture();

public:
	static CNetCapture* instnce();

public:
	virtual bool init();
	virtual size_t getNetCardNum() const;
	virtual bool attach(const std::string & name, input_t input);
	virtual bool detach(const std::string & name, input_t input);

private:
	std::map<std::string, CNetInterface*> m_mapInterface;
};

#endif //__NET_INTERFACE_H__
