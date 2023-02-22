
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

const char *pDstIP = "10.10.10.101";
const char *pSrcIP = "10.10.10.101";

const int dstPort = 9001;
const int srcPort = 9002;

//TCP α???
struct psdHder
{
	unsigned int srcIP;
	unsigned int dstIP;
	unsigned short zero : 8;
	unsigned short proto : 8;
	unsigned short totalLen;
};

unsigned short checkSum(const unsigned char* buffer, int size)
{
	printf("size = %d\n", size);
	unsigned long cksum = 0;
	//此处受大小端影响
	for (int i = 0; i < size / 2; i++)
	{
		cksum += (unsigned short)(buffer[i*2] << 8) + (unsigned short)(buffer[i*2 + 1] & 0x00ff);
	}
	
	if( size % 2 == 1) 
	{ 
		cksum += (unsigned short)(buffer[size - 1] << 8);
	}
	
	while (cksum >> 16) 
	{
		cksum = (cksum >> 16) + (cksum & 0xffff); 
	}
	
	return (unsigned short)(~cksum);
}

unsigned short tcpCheckSum(unsigned char* buffer, const unsigned int size, unsigned int srcip, unsigned int dstip)
{
	unsigned char* pBuf = (unsigned char*)malloc(sizeof(struct psdHder) + size);
	memset(pBuf, 0, sizeof(pBuf));

	struct psdHder* psdHeader = (struct psdHder*)pBuf;
	psdHeader->srcIP = srcip;
	psdHeader->dstIP = dstip;
	psdHeader->zero = 0;
	psdHeader->proto = IPPROTO_TCP;
	psdHeader->totalLen = htons(0x0014);

	memcpy(pBuf + sizeof(struct psdHder), buffer, size);

	unsigned short ret = checkSum(pBuf, sizeof(struct psdHder) + size);

	free(pBuf);
	return ret;
}

int _connect(int sockfd, const char* dstIp, const char* srcIp, const int dstPort, const int srcPort)
{
	unsigned char* buffer = (unsigned char*)malloc(sizeof(struct ip) + sizeof(struct tcphdr));

	memset(buffer, 0, sizeof(buffer));

	struct ip* ipHeader = (struct ip*)buffer;
	ipHeader->ip_v = IPVERSION;
	ipHeader->ip_hl = sizeof(struct ip) / 4;
	ipHeader->ip_tos = 0;
	ipHeader->ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr));
	ipHeader->ip_id = htons(1);
	ipHeader->ip_off = 0;
	ipHeader->ip_ttl = 64;
	ipHeader->ip_p = IPPROTO_TCP;
	ipHeader->ip_sum = 0;
	inet_pton(AF_INET, srcIp, &ipHeader->ip_src.s_addr);
	inet_pton(AF_INET, dstIp, &ipHeader->ip_dst.s_addr);

	ipHeader->ip_sum = htons(checkSum(buffer, sizeof(struct ip)));

	struct tcphdr* tcpHeader = (struct tcphdr*)(buffer + sizeof(struct ip));
	tcpHeader->source = htons(srcPort);
	tcpHeader->dest = htons(dstPort);
	tcpHeader->doff = sizeof(struct tcphdr) / 4;
	tcpHeader->syn = 1;
	tcpHeader->window = htons(4096);
	//tcpHeader->window = 0;
	tcpHeader->check = 0;
	tcpHeader->seq = htonl(1);
	tcpHeader->ack_seq = htonl(0);
	tcpHeader->check = htons(tcpCheckSum((unsigned char*)tcpHeader, sizeof(struct tcphdr), ipHeader->ip_src.s_addr, ipHeader->ip_dst.s_addr));

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, dstIp, &addr.sin_addr.s_addr);
	addr.sin_port = htons(dstPort);

	socklen_t len = sizeof(struct sockaddr_in);

	int send;
	send = sendto(sockfd, buffer, sizeof(struct ip) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, len);
	printf("send %d bytes:\n", send);

	unsigned char rev[1024];
	int n = recvfrom(sockfd, rev, sizeof(rev), 0, NULL, NULL);
	//printf("receive %d bytes:\n", n);

	ipHeader = (struct ip*)rev;

	int ipHeaderlen = (ipHeader->ip_hl & 0x0f) * 4;
	
	tcpHeader = (struct tcphdr*)(rev + ipHeaderlen);

	int sequence = ntohl(tcpHeader->seq);
	int ack = ntohl(tcpHeader->ack_seq);

	printf("ipHeader->ip_len =  %d \n", ntohs(ipHeader->ip_len));
	printf("ACK = %d \n", tcpHeader->ack);
	printf("SYN = %d \n", tcpHeader->syn);
	printf("sequence 0x%x \n", sequence);
	printf("ack = 0x%x \n", ack);
	
	ipHeader = (struct ip*)buffer;
	ipHeader->ip_id = htons(2);;
	ipHeader->ip_sum = 0;
	ipHeader->ip_sum = htons(checkSum(buffer, sizeof(struct ip)));

	tcpHeader = (struct tcphdr*)(buffer + sizeof(struct ip));
	tcpHeader->syn = 0;
	tcpHeader->ack = 1;
	tcpHeader->seq = htonl(2);
	tcpHeader->ack_seq = htonl(sequence + 1);
	tcpHeader->check = 0;
	tcpHeader->check = htons(tcpCheckSum((unsigned char*)tcpHeader, sizeof(struct tcphdr), ipHeader->ip_src.s_addr, ipHeader->ip_dst.s_addr));

	send = sendto(sockfd, buffer, sizeof(struct ip) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, len);
	printf("send %d bytes:\n", send);
	
	char msg[32] = "hello server!";
	unsigned char* smsg = (unsigned char*)malloc(sizeof(struct ip) + sizeof(struct tcphdr) + sizeof(msg));

	ipHeader = (struct ip*)smsg;
	ipHeader->ip_v = IPVERSION;
	ipHeader->ip_hl = sizeof(struct ip) / 4;
	ipHeader->ip_tos = 0;
	ipHeader->ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr) + sizeof(msg));
	ipHeader->ip_id = htons(3);
	ipHeader->ip_off = 0;
	ipHeader->ip_ttl = 64;
	ipHeader->ip_p = IPPROTO_TCP;
	ipHeader->ip_sum = 0;
	inet_pton(AF_INET, srcIp, &ipHeader->ip_src.s_addr);
	inet_pton(AF_INET, dstIp, &ipHeader->ip_dst.s_addr);

	ipHeader->ip_sum = htons(checkSum(smsg, sizeof(struct ip)));

	tcpHeader = (struct tcphdr*)(smsg + sizeof(struct ip));
	tcpHeader->source = htons(srcPort);
	tcpHeader->dest = htons(dstPort);
	tcpHeader->doff = sizeof(struct tcphdr) / 4;
	tcpHeader->syn = 0;
	tcpHeader->window = htons(4096);
	tcpHeader->check = 0;
	tcpHeader->seq = htonl(3);
	tcpHeader->ack_seq = htonl(0);

	memcpy(smsg + sizeof(struct tcphdr) + sizeof(struct ip), msg, sizeof(msg));

	tcpHeader->check = htons(tcpCheckSum((unsigned char*)tcpHeader, sizeof(struct tcphdr) + sizeof(msg), ipHeader->ip_src.s_addr, ipHeader->ip_dst.s_addr));

	send = sendto(sockfd, smsg, sizeof(struct ip) + sizeof(struct tcphdr) + sizeof(msg), 0, (struct sockaddr *)&addr, len);
	printf("sizeof(msg) %d byte:\n", sizeof(msg));

}

int main(int argc, char **argv)
{

	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (sock < 0)
	{
		perror("Socket Error");
		exit(1);
	}

	//IP_HDRINCL, 手动填写IP头
	const int on = 1;
	setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

	const char *query = "I am a Hacker.\n";

	_connect(sock, pDstIP, pSrcIP,dstPort, srcPort);
	
	char buff[3] = {0};
	buff[0] = 0xab;
	buff[1] = 0xbc;
	buff[2] = 0xcd;

//	printf("%x\n", checkSum((unsigned char*)buff, 2));

	//unsigned char rec[1024];
	//n = recvfrom(sock, rec, 1024, 0, NULL, NULL);

	return 0;
}


