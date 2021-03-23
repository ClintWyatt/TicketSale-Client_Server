#ifndef H_TicketClient
#define H_TicketClient
#include <boost/asio.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <string>
#include <boost/array.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <queue>
#include <thread>
#include <iterator>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class TicketClient
{
public:
	TicketClient(boost::asio::io_service &io_service_, boost::asio::ip::tcp::resolver::iterator it);
	~TicketClient();
	void handle_connect(tcp::resolver::results_type::iterator endpoint_iter);
	void udpServer();
	//boost::array <char, 64> message;
	//std::string message;

	/*
	* method takes in the portnumber for the other client and returns either a ticket price from another client or
	* returns 0 (buy from other client failed).
	*/
	unsigned int buyFromScalper(unsigned short portNum, unsigned short balance, std::string ipv4);

	/*
	* Method attempts to sell a ticket to another client
	* Returns either the ticket that the client can buy or zero (other client does not have sufficient funds).
	*/
	unsigned short sellTicket(unsigned short balance);
private:
	void getTicket();
	void read();
	void getSvrResponse();
	char buff[32];
	unsigned short balance = 3000;
	unsigned short udpPort;
	tcp::socket socket;
	boost::asio::ip::tcp::resolver resolver;
	udp::resolver udpResolver;
	boost::asio::io_service& io_service;
	std::queue<unsigned short> wallet;
	int counter;
	std::thread *t1;
};
/*
class UdpServer
{

};
*/
#endif // !H_TicketClient
#pragma once
#pragma once
