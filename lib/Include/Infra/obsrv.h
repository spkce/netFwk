
#ifndef __OBSRV_h__
#define __OBSRV_h__

#include "thread.h"
#include <vector>

namespace Infra
{

template <typename P1, typename P2, typename P3, typename P4>
class CWatcher;

/**
* @brief �۲���ģ����
*/
template <typename P1, typename P2, typename P3, typename P4>
class CObsrv
{
public:
	/**
	* @brief �۲��߻ص�����
	*/
	typedef TFuncation4<void, P1, P2, P3, P4> watch_t;

public:
	/**
	* @brief ���캯��
	* @param maxWather : ���֧��ע��Ĺ۲���
	* @param maxParam : ����������
	*/
	CObsrv(int maxWather, int maxParam);
	virtual ~CObsrv();

	/**
	* @brief �۲��߻ص�����ע��
	* @param fun : �ص�����
	* @return �ɹ�/ʧ��
	*/
	bool attach(const watch_t & fun);

	/**
	* @brief �۲��߻ص�����ע��
	* @param fun : �ص�����
	* @return �ɹ�/ʧ��
	*/
	bool detach(const watch_t & fun);

	/**
	* @brief ֪ͨ�۲���
	* @param  : ֪ͨ����
	*/
	void distribute(P1 p1, P2 p2, P3 p3, P4 p4);

private:
	const int m_maxWather;
	const int m_maxParam;
	CMutex m_mutex;
	std::vector<CWatcher<P1, P2, P3, P4>*> m_vWatcher;
};

}//Infra

#include "obsrv.tpp"

#endif //__OBSRV_h__
