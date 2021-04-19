#include "clientSockets.h"

void UdpClientSockets::removeUdpSocket(unsigned short x)
{
	//std::<unsigned short> iterator* it;
	for (auto it = cliSockets.begin(); it != cliSockets.end(); ++it)
	{
		if (it->first == x)
		{
			cliSockets.erase(it);
			break;
		}
	}
}

void UdpClientSockets::addClientInfo(unsigned short portNum, char ipAddress[])
{
	port_ip clientInfo;
	clientInfo.first = portNum;
	memcpy(clientInfo.second, ipAddress, sizeof(clientInfo.second));
	cliSockets.insert(clientInfo);
}


void UdpClientSockets::otherClientInfo(port_ip& obj)
{
	port_ip otherClient;
	std::set<port_ip>::iterator it = cliSockets.begin();
	while (it != cliSockets.end())
	{
		if (obj.first != it->first)
		{
			obj.first = it->first;
			memcpy(obj.second, it->second, sizeof(obj.second));
			break;
		}
		*it++;
	}
}

unsigned short UdpClientSockets::getNumSockets()
{
	return cliSockets.size();
}
