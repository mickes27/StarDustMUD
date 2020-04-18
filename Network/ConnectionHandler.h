#pragma once
#include "Connection.h"

namespace Network
{

template<typename Protocol, typename Command>
class ConnectionHandler
{

public:

	using conn = Connection<Protocol>;

	ConnectionHandler(conn& conn) : connection(&conn) {}
	virtual ~ConnectionHandler() {};

	virtual void handle(Command data) = 0;
	virtual void enter() = 0;
	virtual void leave() = 0;
	virtual void hungup() = 0;
	virtual void flooded() = 0;

protected:
	conn* connection;
};

}
