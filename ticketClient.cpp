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
io_service(io_service_)
{
	try {
		counter = 0;
		srand(time(NULL));
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
	if (counter == 16) 
	{
		socket.close();
		//code to end the thread (condition variable or sending a message via socket to the udp server).
		t1->join();
		delete t1;
		return; 
	}
	Sleep(1000);
	memset(buff, '\0', sizeof(buff));//null terminating the buff array to prevent garbage values
	sprintf_s(buff, sizeof(buff), "%d", balance);	
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
			if (counter == 1)
			{
				//if the udpServer method is private, the line of code below will not work
				t1 = new std::thread(&TicketClient::udpServer, this);//must delete this
			}
			else
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
	unsigned short udpPort = (rand() % 39000) + 10000;
	boost::asio::io_service ioService;
	udp::endpoint ep;
	boost::system::error_code ec;
	unsigned short ticket;
	udp::socket socket(ioService, udp::endpoint(udp::v4(), udpPort));
	for (; ;)
	{
		memset(cliMsg, '\0', sizeof(cliMsg));
		socket.receive_from(boost::asio::buffer(cliMsg), ep, 0, ec);
		if (!ec)
		{
			ticket = sellTicket(atoi(cliMsg));
			//other client can buy ticket from this client
			if (ticket != 0)
			{
				sprintf_s(cliMsg, sizeof(cliMsg), "%d", ticket);
				socket.send_to(boost::asio::buffer(cliMsg), ep);//sending the ticket to the client
			}
			else
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

unsigned int TicketClient::buyFromScalper(unsigned short portNum, unsigned short balance)
{
	try
	{
		unsigned short ticketPrice;
		boost::system::error_code ec;
		char balance[64];
		memset(balance, '\0', sizeof(balance));
		boost::asio::io_service ioServ;
		udp::resolver resolver(ioServ);
		udp::resolver::query query(udp::v4(), "192.168.0.183", std::to_string(portNum));
		udp::endpoint receiver_endpoint = *resolver.resolve(query);
		udp::socket socket(ioServ);
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
			std::cout << "recieve_from error \n";
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