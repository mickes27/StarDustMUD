#pragma once
#include "DatabaseHandler.h"
#include "../../Entities/Account.h"


namespace GwiezdnyPyl {
	
class AccountHandler : public DatabaseHandler
{
public:
	AccountHandler(SQLite::Database& db) : DatabaseHandler(db) {}

	Entity::ID findByName(const std::string& name) const;
	Entity::ID createAccount(const std::string& name, const std::string& password) const;
	Account getEntity(Entity::ID id) const;

	static bool checkName(const std::string& name);
};

}