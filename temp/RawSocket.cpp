

#include "RawSocket.h"
#include <netinet/if_ether.h>
#include <linux/ip.h>
CRawSocket::CRawSocket()
{

}

CRawSocket::~CRawSocket()
{

}


int CRawSocket::arpRequest()
{
	struct ether_arp* arp_packet = (struct ether_arp *)malloc(sizeof(struct ether_arp));
}

void CRawSocket::input(char* buf, int len)
{
	struct ethhdr* pEthHdr = (struct ethhdr*)buf;
	struct in_mac srcMac;
	struct in_mac dstMac;

	memcpy(&srcMac, pEthHdr->h_source, sizeof(struct in_mac));
	memcpy(&dstMac, pEthHdr->h_dest, sizeof(struct in_mac));

	printf("%s ", INetIF::macFormat(srcMac) );
	printf("-> %s ", INetIF::macFormat(dstMac));
	printf(": 0x%x\n", htons(pEthHdr->h_proto));
	switch (htons(pEthHdr->h_proto))
	{
	case ETH_P_IP:
		//ip_input(buf + sizeof(struct ethhdr), len - sizeof(struct ethhdr) - 4); //-4去掉以太网帧尾
		break;
	case ETH_P_ARP:
		arp_input(buf + sizeof(struct ethhdr), len - sizeof(struct ethhdr) - 4);
		break;
	default:break;
	}
}

void CRawSocket::output(char* buf, int len)
{

}
    


void CRawSocket::arp_output()
{

}

void CRawSocket::ip_input(char* buf, int len)
{
	struct iphdr* pIphdr = (struct iphdr*)buf;

	struct in_addr srcip;
	struct in_addr dstip;

	srcip.s_addr = pIphdr->saddr;
	dstip.s_addr = pIphdr->daddr;

	printf("%s -> ", (char*)inet_ntoa(srcip) );
	printf("%s \n ", (char*)inet_ntoa(dstip) );
	//这种方法会导致srcip和dstip都一样
	//printf("%s -> %s : 0x%x\n", (char*)inet_ntoa(srcip), (char*)inet_ntoa(dstip)); 
}

void CRawSocket::arp_input(char* buf, int len)
{
	struct arphdr* pArphdr = (struct arphdr*)buf;
	printf("----------------arp-------------\n");
	printf("Hardware type:%d\n", htons(pArphdr->ar_hrd));
	printf("Protocol type:0x%x\n", htons(pArphdr->ar_pro));
	printf("Hardware len:%d\n", pArphdr->ar_hln);
	printf("Protocol len:%d\n", pArphdr->ar_pln);
	printf("ARP/RARP operation:%d\n", htons(pArphdr->ar_op));
	
	struct ether_arp* pEtherArp = (struct ether_arp*)buf;
	struct in_mac srcMac;
	struct in_mac dstMac;

	memcpy(&srcMac, pEtherArp->arp_sha, sizeof(struct in_mac));
	memcpy(&dstMac, pEtherArp->arp_tha, sizeof(struct in_mac));

	struct in_addr srcip;
	struct in_addr dstip;

	srcip.s_addr = *((unsigned int*)pEtherArp->arp_spa);
	dstip.s_addr = *((unsigned int*)pEtherArp->arp_tpa);

	printf("Sender MAC:%s \n", INetIF::macFormat(srcMac));
	printf("Sender IP:%s \n", (char*)inet_ntoa(srcip));
	printf("Target MAC:%s \n", INetIF::macFormat(dstMac));
	printf("Target IP:%s \n", (char*)inet_ntoa(dstip));
}

#include <ifaddrs.h>

int getSubnetMask()
{
	struct sockaddr_in *sin = NULL;
	struct ifaddrs *ifa = NULL, *ifList;

	//该方法获取不到mac地址
	if (getifaddrs(&ifList) < 0)
	{
		return -1;
	}

	for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr->sa_family == AF_INET)
		{
			printf("interfaceName: %s \n", ifa->ifa_name);

			sin = (struct sockaddr_in *)ifa->ifa_addr;
			printf("ipAddress: %s \n", inet_ntoa(sin->sin_addr));

			sin = (struct sockaddr_in *)ifa->ifa_dstaddr;
			printf("broadcast: %s \n", inet_ntoa(sin->sin_addr));

			sin = (struct sockaddr_in *)ifa->ifa_netmask;
			printf("subnetMask: %s \n", inet_ntoa(sin->sin_addr));
		}
	}

	freeifaddrs(ifList);

	return 0;
}