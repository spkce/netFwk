#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <string>

namespace NetServer {
	class ISession;
}
namespace Terminal
{

class ITerminal
{
public:
	typedef enum Terminal_t
	{
		emNomral,
	}Terminal_t;

	enum
	{
		emInline,
		emOffline,
	};

protected:
	ITerminal();
	virtual ~ITerminal();
	
public:
	static ITerminal* createTerminal(Terminal_t type);

	virtual bool init() = 0;

	virtual int getState() = 0;

	virtual bool connect(NetServer::ISession* session, int type) = 0;

	virtual bool disconnet(NetServer::ISession* session, int type) = 0;

	virtual bool getVersion(std::string & ver) = 0;

	virtual bool notify(char* buf, int len) = 0;

protected:
	int m_type;
};

} // Terminal
#endif //__TERMINAL_H__
