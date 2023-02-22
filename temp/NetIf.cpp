
#include "NetIf.h"
#include <netinet/ether.h>
#include <netpacket/packet.h> 
#include <ifaddrs.h>
#include <netinet/if_ether.h>

INetIF::INetIF()
{

}

INetIF::~INetIF()
{

}

char* INetIF::macFormat(struct in_mac &mac, char format )
{
	static char _mac[18]; 
	snprintf(_mac, sizeof(_mac), "%02x%c%02x%c%02x%c%02x%c%02x%c%02x",
			mac.buf[0],format,
			mac.buf[1],format,
			mac.buf[2],format,
			mac.buf[3],format,
			mac.buf[4],format,
			mac.buf[5],format
		);
	return _mac;
}

CNetIF::CNetIF()
:INetIF()
,m_socket(-1)
,m_netifNum(0)
{

}


CNetIF::~CNetIF()
{
	if (m_socket > 0)
	{
		close(m_socket);
		m_socket = -1;
	}
}

int CNetIF::init()
{
	//创建原始套接字
	//原始套接字需要一定权限才可穿件成功
	m_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (m_socket < 0)
	{
		return -1;
	}
	return update(m_socket);
}

int CNetIF::update(int socket)
{
	struct ifconf ifc;
	struct ifreq buf[16];
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	
	struct netif_info tempInfo;
	memset(&tempInfo, 0, sizeof(tempInfo));

	if (ioctl(socket, SIOCGIFCONF, (char *)&ifc))
	{
		return -1;
	}

	m_netifNum = ifc.ifc_len / sizeof(struct ifreq);

	for (int i = 0; i < m_netifNum; i++)
	{
		strncpy(tempInfo.name.buf, buf[i].ifr_name, sizeof(struct netif_name));

		if(ioctl(socket, SIOCGIFINDEX, (char *)(&buf[i])))
		{
			continue;
		}
		tempInfo.ifindex = buf[i].ifr_ifindex;
		if (ioctl(socket, SIOCGIFHWADDR, (char *)(&buf[i])))
		{
			continue;
		}
		memcpy(tempInfo.mac.buf, (unsigned char*)buf[i].ifr_hwaddr.sa_data, sizeof(struct in_mac));

		if (ioctl(socket, SIOCGIFADDR, (char *)&buf[i]))
		{
			continue;
		}
		tempInfo.ip = ((struct sockaddr_in *)&(buf[i].ifr_addr))->sin_addr;

		if (ioctl(socket, SIOCGIFNETMASK, &buf[i]))
		{
			continue;
		}
		tempInfo.netmask = ((struct sockaddr_in *)&(buf[i].ifr_netmask))->sin_addr;

		if (ioctl(socket, SIOCGIFBRDADDR, (char *)&buf[i]))
		{
			continue;
		}
		tempInfo.broadcast = ((struct sockaddr_in *)&(buf[i].ifr_broadaddr))->sin_addr;
		
		m_listNetif.push_back(tempInfo);
	}
	return 0;
}

int CNetIF::update()
{
	if (m_socket < 0)
	{
		return -1;
	}
	
	return update(m_socket);
}

int CNetIF::getNetCardNum()
{
	return m_netifNum;
}

int CNetIF::send(char* buf, int len, int ifindex)
{
	if(m_socket < 0)
	{
		return -1;
	}
	struct sockaddr_ll sll;	//原始套接字地址结构
	
	/*将网络接口赋值给原始套接字地址结构*/    
	bzero(&sll, sizeof(sll));    
	sll.sll_ifindex = ifindex;    
	
	// 发送数据 
	return sendto(m_socket, buf, len, 0 , (struct sockaddr *)&sll, sizeof(sll));
}
int CNetIF::attach(INPUT fun)
{
	for (std::list<struct netif_info>::iterator it = m_listNetif.begin(); it != m_listNetif.end(); it++)
	{
		if (it->input != NULL)
		{
			return -1;
		}
	}
	for (std::list<struct netif_info>::iterator it = m_listNetif.begin(); it != m_listNetif.end(); it++)
	{
		it->input = fun;
	}
	return 0;
}
int CNetIF::attach(const char* ifName, INPUT fun)
{
	if(ifName == NULL)
	{
		return -1;
	}

	for (std::list<struct netif_info>::iterator it = m_listNetif.begin(); it != m_listNetif.end(); it++)
	{
		if (strncmp((*it).name.buf, ifName, strlen((*it).name.buf)) == 0)
		{
			if (it->input == NULL)
			{
				it->input = fun;
				return 0;
			}			
		}
	}
	return -1;
}

int CNetIF::detach(INPUT fun)
{
	for (std::list<struct netif_info>::iterator it = m_listNetif.begin(); it != m_listNetif.end(); it++)
	{
		if (it->input == fun)
		{
			it->input = NULL;
		}
	}
	return 0;
}

int CNetIF::detach(const char* ifName, INPUT fun)
{
	if(ifName == NULL)
	{
		return -1;
	}

	for (std::list<struct netif_info>::iterator it = m_listNetif.begin(); it != m_listNetif.end(); it++)
	{
		if (strncmp((*it).name.buf, ifName, strlen((*it).name.buf)) == 0)
		{
			if (it->input == fun)
			{
				it->input = NULL;
				return 0;
			}			
		}
	}
	return -1;
}

int CNetIF::startRecv(const char* ifName)
{
	//没有绑定则收所有网卡信息
	if (ifName == NULL)
	{
		return 0;
	}

	for (std::list<struct netif_info>::iterator it = m_listNetif.begin(); it != m_listNetif.end(); it++)
	{
		struct netif_info &temp = *it;

		if (strncmp(temp.name.buf, ifName, strlen(temp.name.buf)) == 0)
		{
			struct sockaddr_ll addrLL;
			memset(&addrLL, 0, sizeof(struct sockaddr_ll));
			addrLL.sll_family = PF_PACKET;
			addrLL.sll_ifindex = temp.ifindex;
			addrLL.sll_protocol = htons(ETH_P_ALL);
			if (bind(m_socket, (struct sockaddr*)&addrLL, sizeof(addrLL)) < 0)
			{
				break;
			}
			return 0;
		}
	}
	return -1;
}

int CNetIF::recv(char* buf)
{
	if(m_socket < 0)
	{
		return -1;
	}

	struct sockaddr_ll recvAddrLL;
	memset(&recvAddrLL, 0, sizeof(struct sockaddr_ll));
	socklen_t socklen = sizeof(recvAddrLL);
	return recvfrom(m_socket, buf, 1024, 0, (struct sockaddr*)&recvAddrLL, &socklen);
	//return ::recv(m_socket, (void*)buf, sizeof(buf), 0);
}

void CNetIF::Dump()
{
	for (std::list<struct netif_info>::iterator it = m_listNetif.begin(); it != m_listNetif.end(); it++)
	{
		struct netif_info &temp = *it;
		
		printf("device index: %d \n", temp.ifindex);
		printf("device mac: %s \n", macFormat(temp.mac, ':'));
		printf("device ip: %s \n", (char *)inet_ntoa(temp.ip));
		printf("device netmask: %s \n", (char *)inet_ntoa(temp.netmask));
		printf("device broadcast: %s \n", (char *)inet_ntoa(temp.broadcast));
	} 
}