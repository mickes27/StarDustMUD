#pragma once
#include "TelnetHandler.h"
#include "../Entities/Account.h"
#include "../Database/GameDatabase.h"


namespace GwiezdnyPyl {
	
class TelnetMenu : public TelnetHandler::handler
{
	using thandler = TelnetHandler::handler;
public:
	TelnetMenu(Network::Connection<TelnetHandler>& conn, const Entity::ID accountID) :
		thandler(conn),
		account(DatabaseService->AccountDB().getEntity(accountID))
	{}

	void handle(std::string data) override;
	void enter() override { printMenu(); };
	void leave() override {};
	void hungup() override {};
	void flooded() override {};

	void printMenu() const;

protected:
	Account account;		
};

}