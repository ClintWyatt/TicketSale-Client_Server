#include "ticketbooth.h"

/// <summary>
/// Method accepts a shared pointer from the customer session class and
/// uses the customer_ptr (typedef std::shared_ptr<Customer> customer_ptr)
/// as a base class shared ptr to accept the shared pointer from the 
/// customer session class. The shared pointer is then inserted 
/// into a set of customers (std::set<customer_ptr> customers).
/// </summary>
/// <param name="ptr"></param>
void TicketBooth::join(customer_ptr ptr)
{
	customers.insert(ptr);//add the customer to the line
}


std::string TicketBooth::sellTickets(unsigned short balance)
{
	mutex.lock();
	std::string result;
	bool bought = false;
	for (int i = 0; i < ticketPrices.size(); i++)
	{
		if (tickets[i] == 1)
		{
			if (balance > ticketPrices[i])
			{
				result = std::to_string(ticketPrices[i]);//setting the result of the sale to the ticket price
				tickets[i] = 0;
				bought = true;
				break;
			}
		}
	}
	mutex.unlock();
	if (bought == true) { return result; }//return the ticket
	else { return "No!"; }//can't buy the ticket
}


void TicketBooth::setTickets()
{
	if (ticketsLeft == false)
	{
		ticketsLeft = true;
		//should probably put the tickets on the heap in the future
		srand(time(NULL));
		tickets.assign(10000, 1);
		ticketPrices.resize(10000);
		for (int i = 0; i < tickets.size(); i++)
		{
			ticketPrices[i] = (rand() % 100) + 200;
		}
	}
}

void TicketBooth::decideResponse(std::string name)
{
	int i = 0;
	std::string response;
	for (auto customer : customers)//going through all the customers to see if any are trying to buy a ticket
	{
		if ((customer->getStatus()).length() > 0)
		{
			response = sellTickets(atoi(customer->getStatus().data()));//see if the client can buy a ticket from the ticket booth
			customer->setStatus("");//setting the name to nothing to signify that the client does not have a pending response to buy a ticket 
			customer->deliverTicket(response);//attempt to deliver the ticket to the client
			break;
		}
	}
}

int TicketBooth::getCliTicket(unsigned short tkt)
{
	int index = 0;
	for (int i = 0; i < ticketPrices.size(); i++)
	{
		if (tkt == ticketPrices[i])
		{
			tickets[i] = 1;
			index = i;
			break;
		}
	}
	return index;
}

void TicketBooth::leaveBooth(customer_ptr customer)
{
	customers.erase(customer);//remove the customer from the set of customers
}

void TicketBooth::buybackTicket(unsigned short ticket)
{
	for (int i = 0; i < ticketPrices.size(); i++)
	{
		if (ticket == ticketPrices[i])
		{
			tickets[i] = 1;
			break;
		}
	}
}
