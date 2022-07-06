
#ifndef __PACKET_H__
#define __PACKET_H__

//class CSlider
class CPacket
{
public:
	CPacket();
	CPacket(unsigned int size);
	virtual ~CPacket();
public:
	bool append(const char* pbuf, int len);
	char* getBuffer();
	void clear();
	int size() const;
	int capacity() const;
	char operator[](const int n);
private:
	struct Node
	{
		enum
		{
			capacity = 4*1024
		};
		char cap[capacity];
		int use;
		Node* next;
	public:
		Node();
	};

	typedef struct Node Node_t;

	//增加一个尾节点
	Node_t* riseNode(void);
	Node_t* getNodePos(const int n);
	int m_iTotalLen;
	int m_iNode;

	Node_t* m_pHead;
	Node_t* m_pTail;
	Node_t* m_pCurNode;
	char* m_pBuffer;
};

#endif //__PACKET_H__