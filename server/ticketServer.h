#ifndef H_TICKETSERVER
#define H_TICKETSERVER
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <set>
#include <vector>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <iterator>
#include "customer.h"
#include "clientSockets.h"
#include "customerSession.h"
using boost::asio::ip::tcp;


class TicketServer
{
public:
	/*Constructor starts the ticket server

	*/
	TicketServer(boost::asio::io_service& io_service, const tcp::endpoint ep);

	/*Method accepts connections from the clients through acync_connect


	*/
	void connect();

private:
	tcp::socket socket;
	TicketBooth tcketBooth;
	UdpClientSockets clientSkts;
	tcp::acceptor acceptor_;
};
#endif
#pragma once
