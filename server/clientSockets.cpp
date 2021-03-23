#include "clientSockets.h"

void UdpClientSockets::removeUdpSocket(unsigned short x)
{
	//std::<unsigned short> iterator* it;
	for (auto it = cliSockets.begin(); it != cliSockets.end(); ++it)
	{
		if (*it == x)
		{
			cliSockets.erase(it);
			break;
		}
	}
}

void UdpClientSockets::addUdpSocket(unsigned short portNum)
{
	cliSockets.insert(portNum);
}

unsigned short UdpClientSockets::getOtherUdpSocket(unsigned short x)
{
	std::set<unsigned short>::iterator it = cliSockets.begin();
	while (it != cliSockets.end())
	{
		if (x != *it)
		{
			x = *it;
			break;
		}
	}
	return x;
}

unsigned short UdpClientSockets::getNumSockets()
{
	return cliSockets.size();
}