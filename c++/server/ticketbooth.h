#ifndef H_TICKETBOOTH
#define H_TICKETBOOTH
#include "customer.h"
#include <set>
#include <iostream>
#include <vector>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

class TicketBooth
{
public:

	/*
	* putting the client into a set of clients using the abstract class customer via shared ptr
	*/
	void join(customer_ptr ptr);

	/*
	* method will attempt to sell a ticket to a client
	* returns a string that will either be a ticket or "No!" (client failed to buy a ticket)
	*/
	std::string sellTickets(unsigned short balance);

	/*
	* Method buys ticket from the client 
	*/
	void buybackTicket(unsigned short ticket);

	/*
	* Method sets the all the tickets that the ticket booth will sell
	*/
	void setTickets();

	/*
	* method determines the responses for all clients at the ticket booth who have attempted to buy a ticket
	*/
	void decideResponse(std::string name);

	/*
	* Method returns the index of the ticktet that the server buys back from the cli
	*/
	int getCliTicket(unsigned short tkt);

	/*
	* Method will remove the client from the set of clients if the client closed its socket
	*/
	void leaveBooth(customer_ptr customer);


private:
	std::set<unsigned short> udpSockets;
	bool ticketsLeft = false;
	std::set<customer_ptr> customers;
	int counter = 0;
	boost::interprocess::interprocess_mutex mutex;//used for mutual exclusion for the clients seeking to access the tickets
	std::vector<short> tickets;
	std::vector<unsigned short> ticketPrices;
};

#endif
