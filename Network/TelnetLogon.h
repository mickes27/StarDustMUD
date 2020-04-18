#pragma once
#include "TelnetHandler.h"
#include "Connection.h"
#include "../Entities/Entity.h"

namespace GwiezdnyPyl {

enum class LogonState
{
	Intro,
	EnterName,
	EnterPass,
	NewAcc_EnterName,
	NewAcc_EnterPass,
	Dead
};

class TelnetLogon : public TelnetHandler::handler
{
	using thandler = TelnetHandler::handler;
public:
	TelnetLogon(Network::Connection<TelnetHandler>& conn);

	void handle(std::string data) override;
	void enter() override;
	void leave() override {};
	void hungup() override {};
	void flooded() override {};

	static void noRoom(Network::Connection<TelnetHandler>& connection)
	{
		static std::string msg = "Sorry, there is no more room on this server.\r\n";
		connection.send(msg);
	}

	void goToMenu();

private:
	LogonState state;
	int errors;
	Entity::ID accountID;
	std::string accountName;
	std::string accountPass;
};

}