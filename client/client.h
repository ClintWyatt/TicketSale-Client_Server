#ifndef H_CLIENT
#define H_CLIENT
#include <boost/asio.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>


class Client
{
public:
	Client();
	void startSale();

private:
	boost::asio::ip::tcp::resolver resolver;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::ip::tcp::socket socket;
};

#endif // !H_CLIENT
#pragma once
