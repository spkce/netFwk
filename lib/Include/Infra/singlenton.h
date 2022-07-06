#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include "stdio.h"
#include <memory>
#include "thread.h"

#undef PATTERN_SINGLETON_DECLARE
#define PATTERN_SINGLETON_DECLARE(classname)	\
public:											\
	static classname* instance();				\
private:										\
	classname();								\
	~classname();								\
	friend class std::auto_ptr<classname>;		\


#undef PATTERM_SINGLETON_IMPLEMENT
#define PATTERM_SINGLETON_IMPLEMENT(classname)	\
static std::auto_ptr<classname>	instance##classname(NULL);	\
void exit##classname(void)						\
{												\
	instance##classname = std::auto_ptr<classname>(NULL);	\
}												\
classname * classname::instance()						\
{												\
	if (NULL == instance##classname.get())		\
	{											\
		static Infra::CMutex sm_mutex;			\
		Infra::CGuard<Infra::CMutex> guard(sm_mutex);		\
		if (NULL == instance##classname.get())	\
		{										\
			instance##classname = std::auto_ptr<classname>(new classname);	\
			if(0 != atexit(exit##classname))	\
			{									\
				printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);	\
			}									\
		}										\
	}											\
	return instance##classname.get();			\
}												\




#endif //__SINGLETON_H__
