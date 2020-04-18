#pragma once
#include <list>
#include "Connection.h"

namespace Network {

template <class Protocol, class DefaultHandler>
class ConnectionManager
{
	using cList = std::list<typename Connection<Protocol>::handler>;
public:
	ConnectionManager();
	void newConnection(tcp::socket socket);
	void manage();

private:
	int availableConnections() const;
	void send();
	void listen();
	void close();

	cList connections;
	int maxConnections;
};

template <class Protocol, class DefaultHandler>
ConnectionManager<Protocol, DefaultHandler>::ConnectionManager() :
	maxConnections(2)
{
}

template <class Protocol, class DefaultHandler>
void ConnectionManager<Protocol, DefaultHandler>::newConnection(tcp::socket socket)
{
	auto conn = std::make_shared<Connection<Protocol>>(std::move(socket));

	if (availableConnections() == 0) {
		DefaultHandler::noRoom(*conn);
		conn->close();
	} else {
		conn->start();
		connections.emplace_back(conn);

		std::cout << "[INFO] Adding handler" << std::endl;
		auto c = *connections.rbegin();
		c->addHandler(new DefaultHandler(*c));
	}
}

template <class Protocol, class DefaultHandler>
void ConnectionManager<Protocol, DefaultHandler>::manage()
{
	listen();
	send();
	close();
}

template <class Protocol, class DefaultHandler>
int ConnectionManager<Protocol, DefaultHandler>::availableConnections() const
{
	return maxConnections - static_cast<int>(connections.size());
}

template <class Protocol, class DefaultHandler>
void ConnectionManager<Protocol, DefaultHandler>::send()
{
	std::for_each(std::begin(connections), std::end(connections), [] (auto& conn)
	{
		conn->sendData();
	});
}

template <class Protocol, class DefaultHandler>
void ConnectionManager<Protocol, DefaultHandler>::listen()
{
	std::for_each(std::begin(connections), std::end(connections), [](auto& conn)
	{
		conn->receive();
	});
}

template <class Protocol, class DefaultHandler>
void ConnectionManager<Protocol, DefaultHandler>::close()
{
	for (auto conn = std::begin(connections); conn != std::end(connections);) {
		if (!(*conn)->isOpen()) {
			(*conn)->closeSocket();
			conn = connections.erase(conn);
		} else {
			++conn;
		}
	}
}

}
