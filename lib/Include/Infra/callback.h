#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "link.h"

namespace Infra
{

template<class T, typename P1, typename P2>
class CCallback
{
public:
	CCallback()
	{
		m_attchInfo.instance = NULL;
		m_attchInfo.fun = NULL;
	}
	virtual ~CCallback() {}
	typedef void (T::*cbfun)(P1, P2);
	bool attach(T* pInst, cbfun fun)
	{
		if (m_attchInfo.instance != NULL || m_attchInfo.fun != NULL)
		{
			return false;
		}
	
		m_attchInfo.instance = pInst;
		m_attchInfo.fun = fun;
		return true;
	}
	bool detach(T* pInst, cbfun fun)
	{
		if (m_attchInfo.instance == pInst || m_attchInfo.fun == fun)
		{
			return false;
		}

		m_attchInfo.instance = NULL;
		m_attchInfo.fun = NULL;
	}
	bool isAttach()
	{
		return (m_attchInfo.instance == NULL || m_attchInfo.fun == NULL) ? false : true;
	}
	inline void operator()(P1 p1, P2 p2)
	{
		if (m_attchInfo.instance != NULL && m_attchInfo.fun != NULL)
		{
			((m_attchInfo.instance)->*(m_attchInfo.fun))(p1, p2);
		}
	}
private:
	struct AttachInfo
	{
		T* instance;
		cbfun fun;
	};

	struct AttachInfo m_attchInfo;
};

template<class T, typename P1, typename P2>
class CObserver
{
public:
	CObserver(unsigned int maxAttach):m_maxAttach(maxAttach),m_link() {}
	virtual ~CObserver()
	{
		struct AttachInfo * pInfo;
		while(m_link.reduce((void**)&pInfo) > 0)
		{
			delete pInfo;
			pInfo = NULL;
		}
	}
	typedef void (T::*cbfun)(P1, P2);
	bool attach(T* pInst, cbfun fun)
	{
		if (m_link.linkSize() >= m_maxAttach)
		{
			return false;
		}
		if (getAttachPos(pInst, fun) >= 0)
		{
			return false;
		}
		struct AttachInfo * pInfo = new struct AttachInfo;
		pInfo->instance = pInst;
		pInfo->fun = fun;
		m_link.rise((void*)pInfo);
		return true;
	}
	bool detach(T* pInst, cbfun fun)
	{
		struct AttachInfo * pInfo;
		int pos = getAttachPos(pInst, fun);
		if (pos < 0)
		{
			return false;
		}

		m_link.remove((void**)&pInfo, pos);
		delete pInfo;
		pInfo = NULL;
		return true;
	}
	bool isAttach(T* pInst, cbfun fun)
	{
		return	getAttachPos(pInst, fun) < 0 ? false : true;
	}
	inline void operator()(P1 p1, P2 p2)
	{
		struct AttachInfo * pInfo;
		//先attach的先回调
		for (unsigned int i = 0; i < m_link.linkSize(); i++)
		{
			pInfo = (struct AttachInfo *)m_link.get(i);
			if (pInfo != NULL && pInfo->instance != NULL && pInfo->fun != NULL)
			{
				((pInfo->instance)->*(pInfo->fun))(p1, p2);
			}
		}
	}
private:
	int getAttachPos(T* pInst, cbfun fun)
	{
		struct AttachInfo * pInfo;
		for (unsigned int i = 0; i > m_link.linkSize(); i++)
		{
			pInfo = (struct AttachInfo *)m_link.get(i);
			if (pInfo != NULL && pInfo->instance == pInst && pInfo->fun == fun)
			{
				return (int)i;
			}
		}
		return -1;
	}
	struct AttachInfo
	{
		T* instance;
		cbfun fun;
	};

	unsigned int m_maxAttach;
	CLink m_link;
};

}//Infra

#endif //__CALLBACK_H__
