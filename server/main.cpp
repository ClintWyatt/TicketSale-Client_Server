#include <boost/asio.hpp>
#include "ticketServer.h"

int main(int argc, char *argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cout << "usage: <executable> <port number>\n";
			return 0;
		}
		boost::asio::io_service io_service;
		tcp::endpoint endpoint(tcp::v4(), 10000);
		TicketServer server(io_service, endpoint);
		std::cout << "Awaiting clients to buy tiekcts \n";
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << "\n";
	}
	return 0;
}
