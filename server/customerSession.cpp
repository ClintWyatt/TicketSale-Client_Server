#include "customerSession.h"
	
CustomerSession::CustomerSession(TicketBooth& booth, UdpClientSockets& udpCli, tcp::socket socket) :
	tckBooth(booth), socket_(std::move(socket)), cliskts(udpCli)
{
	memset(buff, '\0', sizeof(buff)); 
}

void CustomerSession::startSale()
{
	//shared_from_this
	//Returns a std::shared_ptr<T> that shares ownership of *this with all 
	//existing std::shared_ptr that refer to *this.

	//Effectively executes std::shared_ptr<T>(weak_this), where weak_this is 
	//the private mutable std::weak_ptr<T> member of enable_shared_from_this.
	/*
		Notes
		It is permitted to call shared_from_this only on a previously shared object, 
		i.e.on an object managed by std::shared_ptr(in particular, shared_from_this 
		cannot be called during construction of * this).
		*/
	tckBooth.join(shared_from_this());
	memset(ipAddress, '\0', sizeof(ipAddress));
	rcvClientMssg();//start async operations
}

void CustomerSession::deliverTicket(std::string response_)
{
	sendResponse(response_);
}

std::string CustomerSession::getStatus()
{
	return status;
}

void CustomerSession::setStatus(std::string status_)
{
	status = status_;
}


void CustomerSession::rcvClientMssg()//reads the message from the client in this session
{
	memset(buff, '\0', sizeof(buff));
	message.fill('\0');
	auto self(shared_from_this());//shared_from_this refers to the customer_session class
	boost::asio::async_read(socket_, boost::asio::buffer(buff, sizeof(buff)),
	[this, self](boost::system::error_code ec, std::size_t /*length*/)
	{
		if (!ec)
		{
			//if the udp socket from the client is not registered yet - not known to the server
			if (setUdpSock == false)
			{
				char buffCpy[64];
				memcpy(buffCpy, buff, sizeof(buff));
				char* port;//represents the port number sent by the client
				port = strtok(buffCpy, " \n"); //tokenizing the buffer
				port = strtok(NULL, " \n");	//getting the port number (second integer in the buffer)
				udpSkt = atoi(port);
				port = strtok(NULL, " \n");//getting the IP address
				memcpy(ipAddress, port, sizeof(ipAddress));
				std::cout << "Adding client: " << udpSkt << " - " << ipAddress <<"\n";
				cliskts.addClientInfo(udpSkt, ipAddress);
				setUdpSock = true;
				memset(buff, '\0', sizeof(buff));
				memcpy(buff, buffCpy, 4);//copying only the first 4 bytes of the buffCpy, which is the client balance
				setStatus(buff);//setting the status to the balance from the client. Tells the ticket booth that the client has money to buy a ticket.
			}
			else //client's info is known by the server
			{
				setStatus(buff);//setting the status to the balance from the client. Tells the ticket booth that the client has money to buy a ticket.
			}
			tckBooth.decideResponse(buff);//see if the client can buy a ticket from the ticket booth
		}
		else//client likely closed the socket
		{
			cliskts.removeUdpSocket(udpSkt);//remove the udp socket form the client socket object, shared by all clients by reference
			tckBooth.leaveBooth(shared_from_this());//have the client leave the ticket booth
		}
	});
}

void CustomerSession::sendResponse(std::string response_)
{
	//if the client can buy the ticket form the server
	if (response_ != "No!")
	{
		int ticket = atoi(response_.data());//get the price of the ticket from the client
		memset(buff, '\0', sizeof(buff));//reset the buffer. Prepare to put the ticket into the buffer
		sprintf_s(buff, sizeof(buff), "%d", ticket);//send the udp socket of another client
		//write to the client and then listen for the client's next message with async read from line 80
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(buff), [this, self](boost::system::error_code ec, std::size_t) {

			if (!ec)
			{
				std::cout << "[SERVER] SELL " << buff << " successful! \n";
				rcvClientMssg();
			}
		});
		
	}
	//the clinet cannot buy the ticket from server
	else if (response_ == "No!")
	{
		recieveResponse("No!");
	}
}

void CustomerSession::recieveResponse(std::string response_)
{
	//do synchronous operations here
	memset(buff, '\0', sizeof(buff));
	auto self(shared_from_this());
	
	//if there is more than 1 client buying from the ticket booth
	if (cliskts.getNumSockets() > 1)
	{
		port_ip otherCli;
		cliskts.otherClientInfo(otherCli);//get the udp socket from amother client
		sprintf_s(buff, sizeof(buff), "%d %s", otherCli.first, otherCli.second);
		boost::asio::write(socket_, boost::asio::buffer(buff));//synchronous operation writing to the client
	}
	//only one client at the ticket booth
	else
	{
		memcpy(buff, "BUY FAILED\0", 11);
		boost::asio::write(socket_, boost::asio::buffer(buff));//snchronous operation writing to the client
	}

	boost::asio::read(socket_, boost::asio::buffer(buff));//synchronous operation specifying whether the client bought from another client or failed
	//to get a ticket from another client
	if (memcmp(buff, "BUY SUCCESS", sizeof("BUY SUCCESS")) != 0)//if the client could not buy a ticket from another client
	{
		unsigned short price = atoi(buff);
		std::cout << "[SERVER] Buying back ticktet " << tckBooth.getCliTicket(price) + 1 << " from client\n";
		tckBooth.buybackTicket(atoi(buff));
	}
	rcvClientMssg();
}
