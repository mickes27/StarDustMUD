#include "TelnetLogon.h"
#include "../Database/GameDatabase.h"
#include <fstream>
#include "TelnetMenu.h"

namespace GwiezdnyPyl {

TelnetLogon::TelnetLogon(Network::Connection<TelnetHandler>& conn):
	thandler(conn), 
	accountID(0)
{
	state = LogonState::Intro;
	errors = 0;
}

void TelnetLogon::handle(std::string data)
{
	if (errors == 5)
	{
		connection->getProtocol().sendString(*connection, "<#FF0000>Too many incorrect responses, closing connection...\r\n<#FFFFFF>");
		connection->close();
		return;
	}

	if (state == LogonState::Intro)
	{
		if (boost::algorithm::to_lower_copy(data) == "register") {
			const auto registerFileName = "Data/Logon/register.data";
			std::ifstream f(registerFileName, std::ios::in);
			if (!f.is_open())
			{
				std::cerr << "[FAIL] Cannot open file: " << registerFileName << std::endl;
			}
			std::string str;
			std::getline(f, str, '\0');

			state = LogonState::NewAcc_EnterName;
			connection->getProtocol().sendString(*connection, str);
		} else if (boost::algorithm::to_lower_copy(data) == "login") {
			const auto loginFileName = "Data/Logon/login.data";
			std::ifstream f(loginFileName, std::ios::in);
			if (!f.is_open())
			{
				std::cerr << "[FAIL] Cannot open file: " << loginFileName << std::endl;
			}
			std::string str;
			std::getline(f, str, '\0');

			state = LogonState::EnterName;
			connection->getProtocol().sendString(*connection, str);
		}
	} else if (state == LogonState::EnterName) {
		auto id = DatabaseService->AccountDB().findByName(data);

		if (id == 0) {
			errors++;
			connection->getProtocol().sendString(*connection,
				"<#FF0000>Sorry, the account \"<#FFFFFF>" + data +
				"<#FF0000>\" does not exist.\r\n" +
				"Please enter your name, or \"register\" if you are new: <#FFFFFF>");
		} else {
			state = LogonState::EnterPass;

			accountID = id;
			auto acc = DatabaseService->AccountDB().getEntity(accountID);

			if (acc.isBanned())
			{
				connection->getProtocol().sendString(*connection, "<#FF0000>SORRY! You are BANNED!");
				connection->close();
				state = LogonState::Dead;
				return;
			}

			accountName = acc.getName();
			accountPass = acc.getPassword();

			connection->getProtocol().sendString(*connection, 
				"<#00FF00>Welcome, <#FFFFFF>" + accountName +
				"\r\n<#00FF00>Please enter your password: <#FFFFFF>");
		}
	}
	else if (state == LogonState::EnterPass) {
		if (accountPass == data) {
			connection->getProtocol().sendString(*connection,
				"<#00FF00>Thank you! You are now entering the realm...<#FFFFFF>\r\n");
			goToMenu();
		} else {
			errors++;
			connection->getProtocol().sendString(*connection,
				"<#FF0000>INVALID PASSWORD!\r\n"
				"<#FFFF00>Please enter your password: <#FFFFFF>");
		}
	} else if (state == LogonState::NewAcc_EnterName) {
		auto id = DatabaseService->AccountDB().findByName(data);
		if (id != 0) {
			errors++;
			connection->getProtocol().sendString(*connection,
				"<#FF0000>Sorry, the account name \"<#FFFFFF>" + data +
				"<#FF0000>\" has already been taken.\r\n" +
				"<#FFFF00>Please enter another name: <#FFFFFF>");
		} else {
			if (!DatabaseService->AccountDB().checkName(data)) {
				errors++;
				connection->getProtocol().sendString(*connection,
					"<#FF0000>Sorry, the account name \"<#FFFFFF>" + data +
					"<#FF0000>\" is unacceptible.\r\n" +
					"<#FFFF00>Please enter your desired name: <#FFFFFF>");
			} else {
				state = LogonState::NewAcc_EnterPass;
				accountName = data;
				connection->getProtocol().sendString(*connection,
					"<#00FF00>Please enter your desired password: <#FFFFFF>");
			}
		}
	} else if (state == LogonState::NewAcc_EnterPass) {
		if (data.find_first_of(" \t\n\r") != std::string::npos)
		{
			errors++;
			connection->getProtocol().sendString(*connection,
				"<#FF0000>INVALID PASSWORD!\r\n"
				"<#00FF00>Please enter your desired password: <#FFFFFF>");
			return;
		}

		connection->getProtocol().sendString(*connection, "<#00FF00>Thank you! You are now entering the realm...\r\n<#FFFFFF>");

		auto id = DatabaseService->AccountDB().createAccount(accountName, data);
		accountID = id;

		//TODO: Set admin permissions right now?
		/*if (AccountDB.size() == 0)
			newAccount.SetAccessLevel(Admin);*/

		goToMenu();
	}
}

void TelnetLogon::enter()
{
	const auto logonFileName = "Data/Logon/logon.data";
	std::ifstream f(logonFileName, std::ifstream::in);
	if (!f.is_open())
	{
		std::cerr << "[FAIL] Cannot open file: " << logonFileName  << std::endl;
	}
	std::string str;
	std::getline(f, str, '\0');

	connection->getProtocol().sendString(*connection, str);
}

void TelnetLogon::goToMenu()
{
	connection->removeHandler();
	connection->addHandler(new TelnetMenu(*connection, accountID));
}

}
