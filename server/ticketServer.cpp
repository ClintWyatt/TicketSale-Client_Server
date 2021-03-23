#include "ticketServer.h"

TicketServer::TicketServer(boost::asio::io_service& io_service, const tcp::endpoint ep) : socket(io_service),
		acceptor_(io_service, ep)
	{
		tcketBooth.setTickets();//setting the ticket booth prices. All the clients will reference the same ticketbooth object
		connect();
	}

	void TicketServer::connect()
	{
		acceptor_.async_accept(socket, [this](boost::system::error_code ec)
		{
				if (!ec)
				{
					std::make_shared<CustomerSession>(tcketBooth, clientSkts, std::move(socket))->startSale();
				}
				connect();
		});
	}

	