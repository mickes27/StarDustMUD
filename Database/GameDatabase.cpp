#include "GameDatabase.h"
#include <iostream>

namespace GwiezdnyPyl {

std::unique_ptr<GameDatabase> DatabaseService;

GameDatabase::GameDatabase()
{
	try {
		dataBase = std::make_unique<SQLite::Database>("Database/gwiezdny_pyl.db3", SQLite::OPEN_READWRITE);
		accountHandler = std::make_unique<AccountHandler>(*dataBase);
	}
	catch (std::exception& e) {
		std::cerr << "Cannot connect with database: " << e.what() << std::endl;
		exit(-1);
	}
}

const AccountHandler& GameDatabase::AccountDB() const
{
	return *accountHandler;
}

}
