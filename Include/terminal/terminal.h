#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <string>


namespace NetFwk
{
class ITerminal
{
public:
	enum
	{
		emInline,
		emOffline,
	};

protected:
	ITerminal();
	virtual ~ITerminal();
	
public:
	virtual bool init() = 0;

	virtual int getState() = 0;

	virtual bool getVersion(std::string & ver) = 0;

	virtual bool probe() = 0;

protected:

};

} // NetFwk
#endif //__TERMINAL_H__
