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
					/* the 2 lines of code below also work
					std::shared_ptr<CustomerSession> pt = std::make_shared<CustomerSession>(tcketBooth, clientSkts, std::move(socket));
					pt->startSale();
					*/

					//make a shared_ptr to non-array object
					std::make_shared<CustomerSession>(tcketBooth, clientSkts, std::move(socket))->startSale();
				}
				connect();
		});
	}

	/* make_shared
	*Allocates and constructs an object of type T passing args to its constructor, 
	* and returns an object of type shared_ptr<T> that owns and stores a pointer to it (with a use count of 1).
	* This function uses ::new to allocate storage for the object
	*/

	
