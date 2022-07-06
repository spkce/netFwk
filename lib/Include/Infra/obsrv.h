
#ifndef __OBSRV_h__
#define __OBSRV_h__

#include "thread.h"
#include <vector>

namespace Infra
{

template <typename P1, typename P2, typename P3, typename P4>
class CWatcher;

/**
* @brief 观察者模板类
*/
template <typename P1, typename P2, typename P3, typename P4>
class CObsrv
{
public:
	/**
	* @brief 观察者回调函数
	*/
	typedef TFuncation4<void, P1, P2, P3, P4> watch_t;

public:
	/**
	* @brief 构造函数
	* @param maxWather : 最大支持注册的观察者
	* @param maxParam : 最大参数队列
	*/
	CObsrv(int maxWather, int maxParam);
	virtual ~CObsrv();

	/**
	* @brief 观察者回调函数注册
	* @param fun : 回调函数
	* @return 成功/失败
	*/
	bool attach(const watch_t & fun);

	/**
	* @brief 观察者回调函数注销
	* @param fun : 回调函数
	* @return 成功/失败
	*/
	bool detach(const watch_t & fun);

	/**
	* @brief 通知观察者
	* @param  : 通知参数
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
