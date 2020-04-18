#pragma once
#include <thread>
#include <iostream>
#include "Connection.h"
#include "ConnectionManager.h"

namespace Network {

template <class Protocol, class DefaultHandler>
class Server
{
public:
	Server(short port);
	~Server();

	void setConnectionManager(ConnectionManager<Protocol, DefaultHandler>* manager);
	void startServer();

private:
	void startIOContext();
	void handleAccept(const boost::system::error_code& error);

	boost::asio::io_context ioContext;
	std::thread contextThread;
	tcp::acceptor acceptor;
	tcp::socket listenerSocket;
	ConnectionManager<Protocol, DefaultHandler>* manager;
};

template <class Protocol, class DefaultHandler>
Server<Protocol, DefaultHandler>::Server(const short port) :
	acceptor(ioContext, tcp::endpoint(tcp::v4(), port)),
	listenerSocket(make_strand(ioContext))
{
	startIOContext();
}

template <class Protocol, class DefaultHandler>
Server<Protocol, DefaultHandler>::~Server()
{
	contextThread.join();
}

template <class Protocol, class DefaultHandler>
void Server<Protocol, DefaultHandler>::startIOContext()
{
	contextThread = std::thread([this]
	{
		auto executorGuard = boost::asio::make_work_guard(ioContext);
		ioContext.run();
	});
}

template <class Protocol, class DefaultHandler>
void Server<Protocol, DefaultHandler>::startServer()
{
	acceptor.async_accept(listenerSocket, [this](const boost::system::error_code& error)
	{
		handleAccept(error);
	});
}

template <class Protocol, class DefaultHandler>
void Server<Protocol, DefaultHandler>::handleAccept(const boost::system::error_code& error)
{
	if (error) {
		std::cerr << "[ERROR] Server error: " << error.message() << std::endl;
		return;
	}

	manager->newConnection(std::move(listenerSocket));

	startServer();
}

template <class Protocol, class DefaultHandler>
void Server<Protocol, DefaultHandler>::setConnectionManager(ConnectionManager<Protocol, DefaultHandler>* manager)
{
	this->manager = manager;
}

}
