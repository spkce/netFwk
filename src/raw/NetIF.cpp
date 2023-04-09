
#include "NetIF.h"
#include <netinet/ether.h>
#include <netpacket/packet.h> 
#include <ifaddrs.h>
#include <netinet/if_ether.h>

CMac::CMac()
{

}

CMac::CMac(unsigned char mac[6])
{
	memcpy(m_mac, mac, sizeof(m_mac));
}

CMac::CMac(const char* str, char split)
{
	char format[sizeof("%02x:%02x:%02x:%02x:%02x:%02x")] = {0};
	snprintf(format, sizeof(format), "%s%c%s%c%s%c%s%c%s%c%s", "%02x", split, "%02x", split, "%02x", split, "%02x", split, "%02x", split, "%02x");
	
	unsigned int buf[6] = {0};
	int n = sscanf(str, format, &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5]);
	assert(n != 6);

	for (int i = 0; i < sizeof(m_mac); i++)
	{
		m_mac[i] = (unsigned char)buf[i];
	}
}

CMac::CMac(const CMac & mac)
{
	memcpy(m_mac, mac.m_mac, sizeof(m_mac));
}

CMac::~CMac()
{

}

CMac& CMac::operator=(const CMac& mac)
{
	memcpy(m_mac, mac.m_buf, sizeof(m_mac));
	return *this;
}

bool CMac::operator==(const CMac& mac) const
{
	for (int i = 0; i < sizeof(m_mac); i++)
	{
		if (m_mac[i] != mac.m_buf[i])
		{
			return false;
		}
	}
	return true;
}

const char* CMac::format(char split)
{
	static char _mac[18]; 
	snprintf(_mac, sizeof(_mac), "%02x%c%02x%c%02x%c%02x%c%02x%c%02x",
			m_mac[0],split,
			m_mac[1],split,
			m_mac[2],split,
			m_mac[3],split,
			m_mac[4],split,
			m_mac[5],
		);
	return _mac;
}

class CNetInterface : public NetIfInfo
{
public:
	CNetInterface(const struct NetIfInfo & info);
	virtual ~CNetInterface();

public:
	bool attach(const CNetCapture::input_t & func);
	bool detach(const CNetCapture::input_t & func);
	bool start();
	bool stop();

private:
	void capture(void* arg);

private:
	int m_socket;
	uint32_t m_index;
	uint32_t m_ip;
	uint32_t m_netmask;
	uint32_t m_broadcast;
	CMac m_mac;
	std::string m_name;
	Infra::CThread m_thread;
	CNetCapture::input_t m_input;
	
};

CNetInterface::CNetInterface(const struct NetIfInfo & info)
:m_index(info.index)
,m_ip(info.ip)
,m_netmask(info.netmask)
,m_broadcast(info.broadcast)
,m_mac(info.mac)
,m_name(info.mac)
{
	m_thread.attachProc(Infra::ThreadProc_t(&CNetInterface::capture, this));
	m_thread.createTread();
}

CNetInterface::~CNetInterface()
{
	m_thread.stop(true);
	m_thread.detachProc(Infra::ThreadProc_t(&CNetInterface::capture, this));
}

bool CNetInterface::attach(const CNetCapture::input_t & func)
{
	if (m_input.isEmpty())
	{
		m_input = func;
		return true;
	}

	return false;
}

bool CNetInterface::detach(const CNetCapture::input_t & func)
{
	if (!m_input.isEmpty() && m_input == func)
	{
		m_input.unbind();
		return true;
	}

	return false;
}

bool CNetInterface::start()
{	
	if (m_sockfd >= 0 || m_input.isEmpty())
	{
		return false;
	}

	m_sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (m_sockfd < 0)
	{
		return false;
	}

	struct sockaddr_ll addrLL;
	memset(&addrLL, 0, sizeof(struct sockaddr_ll));
	addrLL.sll_family = PF_PACKET;
	addrLL.sll_ifindex = m_index;
	addrLL.sll_protocol = htons(ETH_P_ALL);
	if (bind(m_sockfd, (struct sockaddr*)&addrLL, sizeof(addrLL)) < 0)
	{
		return false;
	}

	return m_thread.start();
}

bool CNetInterface::stop()
{
	return m_thread.stop();
}

void CNetInterface::capture(void* arg)
{
	if(m_sockfd < 0)
	{
		return -1;
	}

	char buf[1024] = {0};
	struct sockaddr_ll recvAddrLL = {0};
	socklen_t socklen = sizeof(recvAddrLL);
	int len = recvfrom(m_sockfd, buf, 1024, 0, (struct sockaddr*)&recvAddrLL, &socklen);

	if (!m_input.isEmpty())
	{
		m_input(buf, len)
	}
}

CNetCapture::CNetCapture()
{

}

CNetCapture::~CNetCapture()
{

}

static CNetCapture* CNetCapture::instnce();
{
	static CNetCapture instance;
	return &instance;
}

bool CNetCapture::init()
{
	int sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0)
	{
		return -1;
	}

	struct ifconf ifc;
	struct ifreq buf[16];
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;

	if (ioctl(sock, SIOCGIFCONF, (char *)&ifc))
	{
		close(sock);
		return false;
	}

	int num = ifc.ifc_len / sizeof(struct ifreq);

	for (int i = 0; i < num; i++)
	{
		struct NetIfInfo info;
		info.name = buf[i].ifr_name;
		if(ioctl(sock, SIOCGIFINDEX, (char *)(&buf[i])))
		{
			continue;
		}

		info.index = buf[i].ifr_ifindex;
		if (ioctl(sock, SIOCGIFHWADDR, (char *)(&buf[i])))
		{
			continue;
		}

		info.mac = buf[i].ifr_hwaddr.sa_data;
		//memcpy(tempInfo.mac.buf, (unsigned char*)buf[i].ifr_hwaddr.sa_data, sizeof(struct in_mac));

		if (ioctl(sock, SIOCGIFADDR, (char *)&buf[i]))
		{
			continue;
		}
		info.ip = ((struct sockaddr_in *)&(buf[i].ifr_addr))->sin_addr;

		if (ioctl(sock, SIOCGIFNETMASK, &buf[i]))
		{
			continue;
		}
		info.netmask = ((struct sockaddr_in *)&(buf[i].ifr_netmask))->sin_addr;

		if (ioctl(sock, SIOCGIFBRDADDR, (char *)&buf[i]))
		{
			continue;
		}
		info.broadcast = ((struct sockaddr_in *)&(buf[i].ifr_broadaddr))->sin_addr;
		
		m_mapInterface[info.name] = new CNetInterface(info);
	}

	close(sock);
	return true;
}

size_t CNetCapture::getNetCardNum() const
{
	return m_mapInterface.size();
}


bool CNetCapture::attach(const std::string & name, input_t input)
{
	auto it = m_mapInterface.find(name);
	if (it == m_mapInterface.end())
	{
		return false;
	}

	CNetInterface* p = it->second;
	return p->attach(input);
}

bool CNetCapture::detach(const std::string & name, input_t input)
{
	auto it = m_mapInterface.find(name);
	if (it == m_mapInterface.end())
	{
		return false;
	}

	CNetInterface* p = it->second;
	return p->detach(input);
}

void CNetCapture::Dump() const
{
	for (auto it : m_mapInterface)
	{
		CNetInterface p = it.;
		
		printf("device index: %d \n", temp.ifindex);
		printf("device mac: %s \n", macFormat(temp.mac, ':'));
		printf("device ip: %s \n", (char *)inet_ntoa(temp.ip));
		printf("device netmask: %s \n", (char *)inet_ntoa(temp.netmask));
		printf("device broadcast: %s \n", (char *)inet_ntoa(temp.broadcast));
	} 
}