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

unsigned short UdpClientSockets::addUdpSocket()
{

	if (udpSockNum > 49000)
	{
		udpSockNum = 10001;
	}
	else
	{
		udpSockNum++;
	}
	cliSockets.insert(udpSockNum);
	return udpSockNum;
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