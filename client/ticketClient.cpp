#include "ticketClient.h"


struct connectCondition
{
	template<typename Iterator>
	Iterator operator()(const boost::system::error_code ec,
		Iterator next)
	{
		if (ec)std::cout << "Error: " << ec.message() << "\n";
		std::cout << "Trying " << next->endpoint() << "\n";
		return next;
	}
};

TicketClient::TicketClient(boost::asio::io_service &io_service_, boost::asio::ip::tcp::resolver::iterator endpoint) : resolver(io_service_), socket(io_service_),
io_service(io_service_), udpResolver(io_service_)
{
	try {
		counter = 0;
		udpPort = 0;
		handle_connect(endpoint);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

TicketClient::~TicketClient()
{

}

void TicketClient::handle_connect(tcp::resolver::results_type::iterator endpoint_iter)
{
	boost::asio::async_connect(socket, endpoint_iter, [this](boost::system::error_code ec, tcp::resolver::iterator) {
		if (!ec)
		{
			getTicket();
		}
		else
		{
			std::cout << ec.message() << '\n';
		}
	});
	
}

void TicketClient::getTicket()
{
	counter++;
	memset(buff, '\0', sizeof(buff));//null terminating the buff array to prevent garbage values
	if (counter == 16) 
	{
		socket.close();//closing tcp socket
		//std::string ipAddr = boost::asio::ip::address_v4::to_string();
		udp::resolver::query query(udp::v4(), "192.168.0.183", std::to_string(udpPort));
		udp::endpoint receiver_endpoint = *udpResolver.resolve(query);
		udp::socket _socket(io_service);
		_socket.open(udp::v4());
		_socket.send_to(boost::asio::buffer("-1"), receiver_endpoint);//sending -1 to itself to end the udp server thread
		t1->join();
		delete t1;
		return; 
	}
	else if(counter == 1)
	{
		//start udp server thread for this client
		t1 = new std::thread(&TicketClient::udpServer, this);
		Sleep(1000);//sleeping for 1 second to wait for the creation of the udp socket
		sprintf_s(buff, sizeof(buff), "%d %d", balance, udpPort);//copying the balance and udp port into the char array
	}
	else
	{
		Sleep(1000);
		sprintf_s(buff, sizeof(buff), "%d", balance);
	}
	boost::asio::async_write(socket, boost::asio::buffer(buff), [this](boost::system::error_code ec, std::size_t) {
		
		if (!ec)
		{
			std::cout << "[Client->] BUY: " << buff<< '\n';
			read();
		}
		else
		{
			std::cout << "Async Write Error: " << ec.message() << ", closing socket. \n";
			socket.close();
		}	
	});
}

void TicketClient::read()
{
	getSvrResponse();
}

void TicketClient::getSvrResponse()
{
	boost::asio::async_read(socket, boost::asio::buffer(buff), [this](boost::system::error_code ec, std::size_t) {
		if (!ec)
		{
				if (strcmp(buff, "No!") != 0)//client can buy the ticket
				{
					std::cout << "[Server] Buy " << buff << " OK!\n";
					//wallet.push_back(atoi(buff));//add the ticket to the clients wallet
					wallet.push(atoi(buff));
					balance -= atoi(buff);
					getTicket();
				}
				else//client cannot buy the ticket form the server
				{
					std::cout << "[SERVER]: insufficinet funds \n";
					/*
					* Need to insert code to either connect to another client and attempt to buy one of its tickets or
					* resell one of the current client's tickets. If either fail either fail, then either the client failed to buy from the other
					* client failed or there are no other clients buying tickets form the server. If the size of the message is greater than 4, then
					* the server sent the client the udp socket of another client
					*/
					unsigned short tck = wallet.front();
					balance += tck;
					wallet.pop();
					sprintf_s(buff, sizeof(buff), "%d", tck);
					boost::asio::write(socket, boost::asio::buffer(buff));
					getTicket();
				}
			
		}
		else
		{
			std::cout << ec.message() << ", closing socket.\n";
			socket.close();
		}
	});
}

void TicketClient::udpServer()
{
	char cliMsg[64];
	srand(time(NULL));
	udpPort = (rand() % 39000) + 10001;
	udp::endpoint ep;
	boost::system::error_code ec;
	unsigned short ticket;
	try
	{
		udp::socket socket(io_service, udp::endpoint(udp::v4(), udpPort));
		std::cout << "UDP socket at port " << udpPort << "\n";
		for (; ;)
		{
			memset(cliMsg, '\0', sizeof(cliMsg));
			socket.receive_from(boost::asio::buffer(cliMsg), ep, 0, ec);
			if (!ec)
			{
				//if the this client is ending its own thread
				if((memcmp(cliMsg, "-1", 1)) ==0)
				{
					std::cout << "ending udp server thread \n";
					break;
				}
				//getting the result of the sale from another client
				ticket = sellTicket(atoi(cliMsg));

				//other client can buy ticket from this client
				if (ticket > 0)
				{
					sprintf_s(cliMsg, sizeof(cliMsg), "%d", ticket);
					socket.send_to(boost::asio::buffer(cliMsg), ep);//sending the ticket to the client
				}
				else if(ticket == 0)
				{
					sprintf_s(cliMsg, sizeof(cliMsg), "%d", 0);
					socket.send_to(boost::asio::buffer(cliMsg), ep);//sending 0 to the other client, buying failed
				}
			}
			else
			{
				std::cout << "Error reading from udp socket \n";
			}
		}
	}
	catch (std::exception &ec)
	{
		std::cout << ec.what() <<": Error in starting udp server, trying again. \n";
		udpServer();
	}
}

unsigned int TicketClient::buyFromScalper(unsigned short portNum, unsigned short balance, std::string ipv4)
{
	try
	{
		unsigned short ticketPrice;
		boost::system::error_code ec;
		char balance[64];
		memset(balance, '\0', sizeof(balance));
		udp::resolver::query query(udp::v4(), "192.168.0.183", std::to_string(portNum));
		udp::endpoint receiver_endpoint = *udpResolver.resolve(query);
		udp::socket socket(io_service);
		socket.open(udp::v4());
		sprintf_s(balance, sizeof(balance), "%d", balance);
		socket.send_to(boost::asio::buffer(balance), receiver_endpoint);//send balance to other client
		socket.receive_from(boost::asio::buffer(balance), receiver_endpoint, 0, ec);
		if (!ec)
		{
			ticketPrice = atoi(balance);
			if (ticketPrice > 0)
			{
				return ticketPrice;
			}
		}
		else
		{
			std::cout << "recieve_from error - method buyFromScalper \n";
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << "\n";
	}
	return 0;//client failed to buy from other client
}

unsigned short TicketClient::sellTicket(unsigned short balance)
{
	unsigned short ticket;
	for (auto a = wallet.front(); a != wallet.back(); ++a)
	{
		if (balance >= a)
		{
			ticket = a;
			return ticket;//return the ticket
		}
	}
	return 0;//buying the ticket failed
}