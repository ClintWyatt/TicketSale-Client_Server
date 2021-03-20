#ifndef H_CLIENTSOCKETS
#define H_CLIENTSOCKETS
#include <vector>
#include <iterator>
#include <set>

class UdpClientSockets
{
public:
	/*
	* Method adds a socket to the set of sockets
	* The return value is a socket not used by anyother client
	*/
	unsigned short addUdpSocket();

	/*
	*
	*/
	void removeUdpSocket(unsigned short x);

	/*
	* Method returns a udp socket of another client if the number of clinets buying tickets is greater than 2.
	*/
	unsigned short getOtherUdpSocket(unsigned short x);

	/*
	* Method returns the number of udp sockets (represents other clients)
	*/
	unsigned short getNumSockets();
private:
	std::set<unsigned short> cliSockets;
	unsigned short udpSockNum = 10000;
};
#endif
#pragma once
