#include <iostream>
#include "Network/Server.h"
#include "Network/TelnetHandler.h"
#include "Network/ConnectionManager.h"
#include "Network/TelnetLogon.h"
#include "Database/GameDatabase.h"

using namespace GwiezdnyPyl;
using namespace Network;

int main() {
	std::cout << "Gwiezdny Pyl MUD v0.1" << std::endl;

	try {
		DatabaseService = std::make_unique<GameDatabase>();

		Server<TelnetHandler, TelnetLogon> server(23);
		ConnectionManager<TelnetHandler, TelnetLogon> connectionManager;

		server.setConnectionManager(&connectionManager);
		server.startServer();

		while(true) {
			connectionManager.manage();
		}

	} catch (...) {
		std::cout << "[ERROR] Cos jeblo..." << std::endl;
	}

	return 0;
}
