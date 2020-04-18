#pragma once
#include "Connection.h"
#include "ConnectionHandler.h"


namespace GwiezdnyPyl {

class TelnetHandler
{
public:
	using handler = Network::ConnectionHandler<TelnetHandler, std::string>;
	using connection = Network::Connection<TelnetHandler>;

	void translate(connection& conn, const std::string& buffer);
	void sendString(connection& conn, const std::string& string);

	static std::string translateColors(const std::string& str);
	static void translateStringColor(std::string::size_type i, std::string::size_type j, std::string& str);
	static void translateNumberColor(std::string::size_type i, std::string::size_type j, std::string& str);
};

}