#include <boost/asio.hpp>
#include "ticketServer.h"

int main()
{
	boost::asio::io_service io_service;
	tcp::endpoint endpoint(tcp::v4(), 10000);
	TicketServer server(io_service, endpoint);
	std::cout << "Awaiting clients to buy tickets \n";
	io_service.run();
	return 0;
}
