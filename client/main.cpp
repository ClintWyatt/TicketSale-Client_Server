#include "ticketClient.h"
#include <boost/asio.hpp>
#include <iostream>
using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
	try
	{
		if (argc != 4)
		{
			std::cout << "Usage: <executable> <host ip> <host port> <own ip>\n";
			return 0;
		}
		
		boost::asio::io_service io_service;
		TicketClient tclient(io_service, argv[1], argv[2], argv[3]);
		io_service.run();
	}
	catch (boost::system::error_code ec)
	{
		std::cout << ec.message() << "\n";
	}

	return 0;
}
