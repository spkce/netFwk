#include <map>
#include "thread.h"
#include "protocol.h"

namespace NetFwk
{



IProtocol::IProtocol()
{

}

IProtocol::~IProtocol()
{

}

void IProtocol::getVersion(std::string & ver)
{
	ver = m_version;
}

} //NetFwk
