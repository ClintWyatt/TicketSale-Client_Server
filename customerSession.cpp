#include "customerSession.h"

CustomerSession::CustomerSession(TicketBooth& booth, UdpClientSockets& udpCli, tcp::socket socket) :
	tckBooth(booth), socket_(std::move(socket)), cliskts(udpCli)
{
	memset(buff, '\0', sizeof(buff));
	udpSkt = cliskts.addUdpSocket();//udpSkt represent the client's udp socket that will be used to listen for other buy requests from other clients 
}

void CustomerSession::startSale()
{
	tckBooth.join(shared_from_this());
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


void CustomerSession::rcvClientMssg()//reads the message from the clinet in this session
{
	memset(buff, '\0', sizeof(buff));
	message.fill('\0');
	auto self(shared_from_this());//shared_from_this refers to the customer_session class
	boost::asio::async_read(socket_,
		boost::asio::buffer(buff, sizeof(buff)),
		[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				setStatus(buff);//setting the status to the balance from the client. Tells the ticket booth that the client has money to buy a ticket.
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
		if (cliUdpSock == false)//if the client does not have its udp server started yet
		{
			sprintf_s(buff, sizeof(buff), "%d %d", ticket, cliskts.getOtherUdpSocket(udpSkt));
			cliUdpSock = true;
		}
		else//client has its udp server running
		{
			sprintf_s(buff, sizeof(buff), "%d", ticket);//send the udp socket of another client
		}

		//write to the client and then listen for the client's next message with async read from line 80
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(buff), [this, self](boost::system::error_code ec, std::size_t) {

			if (!ec)
			{
				std::cout << "[SERVER] SELL " << buff << " successful! \n";
			}
			});
		rcvClientMssg();
	}
	else if (response_ == "No!")//the clinet cannot buy the ticket from server
	{
		recieveResponse("No!");
	}
}

void CustomerSession::recieveResponse(std::string response_)
{
	//do synchronous operations here
	memset(buff, '\0', sizeof(buff));
	memcpy(buff, "No!\0", 5);
	auto self(shared_from_this());
	//need to insert the code to determine whether the client can buy from other clients via udp
	if (cliskts.getNumSockets() > 1)
	{
		boost::asio::write(socket_, boost::asio::buffer(buff));//synchronous operation writing to the client
	}
	else
	{
		boost::asio::write(socket_, boost::asio::buffer(buff));//synchronous operation writing to the client
	}

	boost::asio::read(socket_, boost::asio::buffer(buff));//synchronous operation specifying whether the client bought from another client or failed
	//to get a ticket from another client
	if (memcmp(buff, "BUY FAILED", sizeof("BUY FAILED")) != 0)//if the client could not buy a ticket from another client
	{
		unsigned short price = atoi(buff);
		std::cout << "[SERVER] Buying back ticktet " << tckBooth.getCliTicket(price) + 1 << " from client\n";
		tckBooth.buybackTicket(atoi(buff));
	}
	rcvClientMssg();
}
