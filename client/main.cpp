#include "ticketClient.h"
#include <boost/asio.hpp>
#include <iostream>
using boost::asio::ip::tcp;
std::string message;
int main()
{
	
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);
	auto endpoint_iterator = resolver.resolve("192.168.0.183", "10000");
	unsigned int portnum = 10000;
	TicketClient tclient(io_service, endpoint_iterator);
	io_service.run();
	
	/*
	try
	{
		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query("127.0.0.1", "10000");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		std::string message;
		tcp::socket socket(io_service);
		boost::asio::connect(socket, endpoint_iterator);
		socket.send(boost::asio::buffer("4000"));
		socket.receive(boost::asio::buffer(message));
		std::cout << message << '\n';
	}
	catch (boost::system::error_code ec)
	{
		std::cout << ec.message() << '\n';
	}
	*/
	return 0;
}