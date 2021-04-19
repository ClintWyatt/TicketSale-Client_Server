#ifndef H_CLIENTSOCKETS
#define H_CLIENTSOCKETS
#include <vector>
#include <iterator>
#include <set>
#include <utility>
#include <algorithm>

typedef std::pair<unsigned short, char[16]> port_ip; 

class UdpClientSockets
{
public:
	/*
	* Method adds a socket to the set of sockets
	* The return value is a socket not used by anyother client
	*/
	void addClientInfo(unsigned short portNum, char ipAddress[]);

	/*
	* removes udp socket from a client that is leaving the ticket booth
	*/
	void removeUdpSocket(unsigned short x);

	/*
	* Method returns a udp socket of another client if the number of clinets buying tickets is greater than 2.
	*/
	void otherClientInfo(port_ip &obj);

	/*
	* Method returns the number of udp sockets (represents other clients)
	*/
	unsigned short getNumSockets();
private:
	std::set<port_ip> cliSockets;
	unsigned short udpSockNum = 10000;
};
#endif
#pragma once
