#include "ticketClient.h"

TicketClient::TicketClient(boost::asio::io_service &io_service_, char* hostIp, char *portNum, char* ownIp) : io_service(io_service_), 
socket(io_service_), resolver(io_service_), udpResolver(io_service_), signals(io_service_, SIGINT), timer(io_service_, boost::posix_time::seconds(1))
{
	try
	{
		counter = 0;
		udpPort = 0;
		memcpy(ownIP, ownIp, strnlen_s(ownIp, 19));
		tcp::resolver::query query(hostIp, portNum);
		tcp::resolver::iterator endpoint_itterator = resolver.resolve(query);
		handle_connect(endpoint_itterator);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
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
			//getTicket();
			timer.async_wait(boost::bind(&TicketClient::getTicket, this));
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
		endUdpServer();
		t1->join();
		delete t1;
		io_service.stop();//end io_service for sigint since no signal was sent to this process
		return; 
	}
	else if(counter == 1)
	{
		//start udp server thread for this client
		t1 = new std::thread(&TicketClient::udpServer, this);
		Sleep(1000);//sleeping for 1 second to wait for the creation of the udp socket
		sprintf_s(buff, sizeof(buff), "%d %d %s", balance, udpPort, ownIP);//copying the balance, udp port, and local IP address into the char array

		//code below will catch a sigint if the user presses ctrl+c on keyboard
		signals.async_wait([this](const boost::system::error_code& error, int signalNumber) {
			if (!error)
			{
				std::cout << "SIGINT caught. Ending sale for client \n";
				socket.close();
				endUdpServer();
				t1->join();
				delete t1;
				exit(1);//exit the program
			}
			else
			{
				std::cout << "SIGINT - unknown error\n";
			}
		});
	}
	else
	{
		sprintf_s(buff, sizeof(buff), "%d", balance);
	}
	
	boost::asio::async_write(socket, boost::asio::buffer(buff), [this](boost::system::error_code ec, std::size_t bytes_transfered) {
		
		if (!ec)
		{
			std::cout << "[Client->] BUY: $" << balance<< '\n';
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
	boost::asio::async_read(socket, boost::asio::buffer(buff), [this](boost::system::error_code ec, std::size_t bytes_read) {
		if (!ec)
		{
			//client can buy the ticket
				if ((strlen(buff)) < 4)
				{
					std::cout << "[Server] Buy $" << buff << " OK!\n";
					//wallet.push_back(atoi(buff));//add the ticket to the clients wallet
					wallet.push(atoi(buff));
					balance -= atoi(buff);
				}
				//client could not buy from the server and there are not other clients buying tickets
				else if ((strcmp(buff, "BUY FAILED")) == 0)
				{
					std::cout << "[SERVER]: insufficinet funds.  \n";
					unsigned short ticket = sellTicketToSvr();
					std::cout << "Selling ticket with price " << ticket << " back to server \n";
					memset(buff, '\0', sizeof(buff));
					sprintf_s(buff, sizeof(buff), "%d", ticket);
					//send the ticket back to the server
					boost::asio::write(socket, boost::asio::buffer(buff));
				}
				//client cannot buy the ticket form the server and there are other clients 
				else
				{
					char otherIp[20];
					memset(otherIp, '\0', sizeof(otherIp));
					std::cout << "[SERVER]: insufficinet funds \n";
					/*
					* Need to insert code to either connect to another client and attempt to buy one of its tickets or
					* resell one of the current client's tickets. If either fail either fail, then either the client failed to buy from the other
					* client failed or there are no other clients buying tickets form the server. If the size of the message is greater than 4, then
					* the server sent the client the udp socket of another client
					*/
					char* token = strtok(buff, " \n");
					unsigned short udpPort = atoi(token);
					token = strtok(NULL, " \n");
					memcpy(otherIp, token, sizeof(otherIp));
					//if the client was able to buy from the scalper (other client)
					if ((buyFromScalper(udpPort, balance, otherIp)) > 0)
					{
						//tell the server that the client was able to buy from another client
						memcpy(buff, "BUY SUCCESS", sizeof("BUY SUCCESS"));
					}
					else
					{
						unsigned short tck = wallet.front();
						balance += tck;
						wallet.pop();
						sprintf_s(buff, sizeof(buff), "%d", tck);
					}
					boost::asio::write(socket, boost::asio::buffer(buff));
				}
				//ensure that the timer does not drift away from the whole-second mark due to any delays in processing the handler.
				timer.expires_at(timer.expires_at() + boost::posix_time::seconds(1));
				timer.async_wait(boost::bind(&TicketClient::getTicket, this));
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
	char cliMsg[64];//message from the other client
	srand(time(NULL));
	udpPort = (rand() % 39000) + 10001;
	udp::endpoint ep;
	boost::system::error_code ec;
	boost::asio::io_service io_serv;
	unsigned short ticket;
	try
	{
		udp::socket _socket(io_serv, udp::endpoint(udp::v4(), udpPort));
		//std::cout << "UDP socket at port " << udpPort << "\n";
		for (; ;)
		{
			memset(cliMsg, '\0', sizeof(cliMsg));
			_socket.receive_from(boost::asio::buffer(cliMsg), ep, 0, ec);
			if (cliMsg[0] != '-') { std::cout << "[OTHER CLIENT]->BUY $" << cliMsg << "\n"; }
			if (!ec)
			{
				//if the this client is ending its own thread
				if((memcmp(cliMsg, "-1", 1)) ==0)
				{
					socket.close();//closing socket to UDP server
					break;
				}

				//getting the result of the sale from another client
				ticket = sellTicket(atoi(cliMsg));

				//other client can buy ticket from this client
				if (ticket > 0)
				{
					sprintf_s(cliMsg, sizeof(cliMsg), "%d", ticket);
					balance += ticket;
					_socket.send_to(boost::asio::buffer(cliMsg), ep);//sending the ticket to the client
				}
				//other client cannot buy from this client
				else if(ticket == 0)
				{
					sprintf_s(cliMsg, sizeof(cliMsg), "%d", 0);
					_socket.send_to(boost::asio::buffer(cliMsg), ep);//sending 0 to the other client, buying failed
				}
			}
			else
			{
				std::cout << "Error reading from udp socket \n";
			}
		}
		_socket.close();
	}
	catch (std::exception &ec)
	{
		std::cout << ec.what() <<": Error in starting udp server, trying again. \n";
		udpServer();//trying this method again
	}
}

unsigned int TicketClient::buyFromScalper(unsigned short portNum, unsigned short balance, char ipv4[])
{
	try
	{
		unsigned short ticketPrice;
		boost::system::error_code ec;
		char buffer[64];
		memset(buffer, '\0', sizeof(buffer));
		udp::resolver::query query(udp::v4(), ipv4, std::to_string(portNum));
		udp::endpoint receiver_endpoint = *udpResolver.resolve(query);
		udp::socket _socket(io_service);
		_socket.open(udp::v4());
		sprintf_s(buffer, sizeof(buffer), "%d", balance);
		_socket.send_to(boost::asio::buffer(buffer), receiver_endpoint);//send balance to other client
		_socket.receive_from(boost::asio::buffer(buffer), receiver_endpoint, 0, ec);
		if (!ec)
		{
			//if the client failed to buy a ticket from the scalper (other client)
			if (buffer[0] == '0') 
			{ 
				std::cout << "[SCALPER]-> failed to buy ticket \n"; 	
			}
			else
			{
				ticketPrice = atoi(buffer);
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

unsigned short TicketClient::sellTicketToSvr()
{
	unsigned short ticket = wallet.front();
	wallet.pop();
	return ticket;
}

void TicketClient::endUdpServer()
{
	try
	{
		udp::resolver::query query(udp::v4(), ownIP, std::to_string(udpPort));
		udp::endpoint receiver_endpoint = *udpResolver.resolve(query);
		udp::socket _socket(io_service);
		_socket.open(udp::v4());
		memcpy(buff, "-1", 2);
		_socket.send_to(boost::asio::buffer(buff), receiver_endpoint);//sending -1 to itself to end the udp server thread
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << " - EndUdpServer method\n";
	}
}
