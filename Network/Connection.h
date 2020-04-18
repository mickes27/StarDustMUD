#pragma once
#include <boost/asio.hpp>
#include <string>
#include <deque>
#include <iostream>
#include <stack>
#include <mutex>
#include <boost/algorithm/string.hpp>

namespace Network {

using boost::asio::ip::tcp;

template <class Protocol>
class Connection : public std::enable_shared_from_this<Connection<Protocol>>
{
public:
	using handler = std::shared_ptr<Connection>;

	Connection(tcp::socket socket);

	void start();
	tcp::socket& getSocket();
	Protocol& getProtocol();
	bool isOpen() const;
	void bufferData(const std::string& buffer);
	void sendData();
	void receive();
	void close();
	void closeSocket();

	void send(const std::string& message);

	typename Protocol::handler* getHandler();
	void clearHandlers();
	void addHandler(class Protocol::handler* handler);
	void removeHandler();
	void switchHandler(class Protocol::handler* handler);

private:
	void send();
	void handleWrite(const boost::system::error_code& error, size_t bytesTransferred);
	void read();
	void handleRead(const boost::system::error_code& error, size_t bytesTransferred);

	tcp::socket socket;
	Protocol protocol;
	std::deque<std::string> sendBuffer;
	std::deque<std::string> readBuffer;
	std::vector<char> newestMessage;
	std::stack<class Protocol::handler*> handlerStack;
	bool isSending;
	bool isClosed;
	std::mutex dequeMtx;
};

template <class Protocol>
Connection<Protocol>::Connection(tcp::socket socket) :
	socket(std::move(socket)),
	newestMessage(1024),
	isSending(false),
	isClosed(false)
{
}

template <class Protocol>
void Connection<Protocol>::start()
{
	read();
}

template <class Protocol>
tcp::socket& Connection<Protocol>::getSocket()
{
	return socket;
}

template <class Protocol>
Protocol& Connection<Protocol>::getProtocol()
{
	return protocol;
}

template <class Protocol>
bool Connection<Protocol>::isOpen() const
{
	return !isClosed && socket.is_open();
}

template <class Protocol>
void Connection<Protocol>::bufferData(const std::string& buffer)
{
	std::lock_guard<std::mutex> lock(dequeMtx);
	sendBuffer.push_back(buffer);
}

template <class Protocol>
void Connection<Protocol>::sendData()
{
	std::unique_lock<std::mutex> lock(dequeMtx);
	if (isSending || sendBuffer.size() == 0) {
		return;
	}
	lock.unlock();

	std::cout << "[INFO] Starting send..." << std::endl;
	send();
}

template <class Protocol>
void Connection<Protocol>::receive()
{
	std::unique_lock<std::mutex> lock(dequeMtx);
	if (!readBuffer.empty()) {
		const auto message = readBuffer[0];
		readBuffer.pop_front();
		lock.unlock();
		std::cout << "[INFO] Passing: " << message << std::endl;
		protocol.translate(*this, message);
	}
}

template <class Protocol>
void Connection<Protocol>::close()
{
	isClosed = true;
}

template <class Protocol>
void Connection<Protocol>::closeSocket()
{
	boost::system::error_code error;
	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
	std::cout << "[INFO] Shutdown socket: " << error.message() << std::endl;
	socket.close(error);
	std::cout << "[INFO] Close socket: " << error.message() << std::endl;
	clearHandlers();
}

template <class Protocol>
void Connection<Protocol>::send(const std::string& message)
{
	bufferData(message);
	sendData();
}

template <class Protocol>
void Connection<Protocol>::send()
{
	isSending = true;
	std::unique_lock<std::mutex> lock(dequeMtx);
	const std::string& message = sendBuffer[0];
	lock.unlock();
	std::cout << "[INFO] Message to send: " << message << std::endl;
	boost::asio::async_write(socket, boost::asio::buffer(message), 
		[self = this->shared_from_this()] (const boost::system::error_code& error, size_t bytesTransferred)
		{
			self->handleWrite(error, bytesTransferred);
		}
	);
}

template <class Protocol>
void Connection<Protocol>::handleWrite(const boost::system::error_code& error, size_t bytesTransferred)
{
	std::unique_lock<std::mutex> lock(dequeMtx);
	sendBuffer.pop_front();
	lock.unlock();

	if (error) {
		std::cerr << "[ERROR] Cannot send: " << error.message() << std::endl;
	}

	if (!sendBuffer.empty()) {
		send();
	} else {
		isSending = false;
	}
}

template <class Protocol>
void Connection<Protocol>::read()
{
	socket.async_read_some(boost::asio::buffer(newestMessage), 
		[self = this->shared_from_this()](const boost::system::error_code& error, size_t bytesTransferred)
		{
			self->handleRead(error, bytesTransferred);
		}
	);
}

template <class Protocol>
void Connection<Protocol>::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (error) {
		std::cerr << "[ERROR] Cannot read: " << error.message() << std::endl;
		return;
	}

	const auto message = boost::trim_copy(std::string(std::begin(newestMessage), std::begin(newestMessage) + bytesTransferred));
	std::cout << "[INFO] Received: " << message << std::endl;
	std::unique_lock<std::mutex> lock(dequeMtx);
	readBuffer.push_back(message);
	lock.unlock();
	read();
}

template <class Protocol>
typename Protocol::handler* Connection<Protocol>::getHandler()
{
	if (handlerStack.empty()) {
		return nullptr;
	}
	return handlerStack.top();
}

template <class Protocol>
void Connection<Protocol>::clearHandlers()
{
	if (getHandler()) {
		getHandler()->leave();
	}

	while (getHandler()) {
		delete getHandler();
		handlerStack.pop();
	}
}

template <class Protocol>
void Connection<Protocol>::addHandler(typename Protocol::handler* handler)
{
	std::cout << "[INFO] Checking if handler exists" << std::endl;
	if (getHandler()) {
		std::cout << "[INFO] Leaving, cause exists" << std::endl;
		getHandler()->leave();
	}

	std::cout << "[INFO] Pushing new handler" << std::endl;
	handlerStack.push(handler);
	std::cout << "[INFO] Doing enter" << std::endl;
	handler->enter();
}

template <class Protocol>
void Connection<Protocol>::removeHandler()
{
	getHandler()->leave();
	delete getHandler();
	handlerStack.pop();
	if (getHandler()) {
		getHandler()->enter();
	}
}

template <class Protocol>
void Connection<Protocol>::switchHandler(typename Protocol::handler* handler)
{
	if (getHandler())
	{
		getHandler()->leave();
		delete getHandler();
		handlerStack.pop();
	}

	handlerStack.push(handler);
	handler->Enter();
}

}
