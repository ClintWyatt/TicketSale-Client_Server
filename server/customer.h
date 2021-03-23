#ifndef H_CUSTOMER
#define H_CUSTOMER
#include <string>
#include <boost/smart_ptr/shared_ptr.hpp>
class Customer
{
public:
	virtual void startSale() = 0;
	virtual std::string getStatus() = 0;
	virtual void setStatus(std::string status_) = 0;
	virtual void deliverTicket(std::string response_) = 0;
};

typedef std::shared_ptr<Customer> customer_ptr;
#endif // !H_CUSTOMER
#pragma once
