#pragma once
#include <memory>
#include <SQLiteCpp/Database.h>
#include "Handlers/AccountHandler.h"

namespace GwiezdnyPyl {

extern std::unique_ptr<class GameDatabase> DatabaseService;

class GameDatabase
{
public:
	GameDatabase();

	const AccountHandler& AccountDB() const;

private:
	std::unique_ptr<SQLite::Database> dataBase;
	std::unique_ptr<AccountHandler> accountHandler;
};

}
