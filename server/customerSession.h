#ifndef H_CUSTOMERSESSION
#define H_CUSTOMERSESSION
#include "customer.h"
#include "clientSockets.h"
#include "ticketbooth.h"
#include <string>
#include <boost/array.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class CustomerSession : public Customer, public std::enable_shared_from_this<CustomerSession>
{
public:
	/*
	constructor initializes the ticket booth shared by all the clients and initializes the socket to be used by all the clients
	*/
	CustomerSession(TicketBooth& booth, UdpClientSockets& udpCli, tcp::socket socket);

	/*
	* Method will start the ticket sale to all clients who are connected to the server
	*/
	void startSale();

	/*
	* Method delivers the response to the client. The response will tell the client whether the sale was sucessful or not.
	*/
	void deliverTicket(std::string response_);

	/*
	* Method is used to determine whether a client has attempted to buy a ticket form the server. If the status is not empty, then
	* the client has attempted to buy a ticket from the ticket server.
	*/
	std::string getStatus();

	/*
	* Method is used to either clear the status after the client has bought the ticket or populate the status string after the client
	* has attempted to buy the ticket form the server. In the case that the client has attempted to buy a ticket from the server,
	* the status string will either have a string representation of the ticket or have the string "No!" to indicate the sale failed.
	*/
	void setStatus(std::string status_);


private:
	/*
	* method recives a message from the client with the client's balance. The balance is used to buy a ticket.
	*/
	void rcvClientMssg();

	/*
	* Method is used to send the result of the sale. The sale will either be the ticket price or "No! xxxx", with xxxx being the port of
	* an available udp socket for another client
	*/
	void sendResponse(std::string response_);

	/*
	* Method is used if the client cannot buy tickets from the server
	*/
	void recieveResponse(std::string response_);
	bool setUdpSock = false;
	unsigned short udpSkt;
	char buff[32];
	std::string status;
	tcp::socket socket_;
	boost::array<char, 32> message;
	std::string buffer;//represents the balance from the client and the response to the client
	TicketBooth& tckBooth;//reference to the ticket booth class for the ticket server
	UdpClientSockets& cliskts;//refernce to all the client sockets
};
#endif // !H_CUSTOMERSESSION
#pragma once
