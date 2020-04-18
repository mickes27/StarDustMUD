#include "AccountHandler.h"
#include <iostream>

namespace GwiezdnyPyl {

Entity::ID AccountHandler::findByName(const std::string& name) const
{
	try {
		const auto findQuery = "SELECT ID FROM accounts WHERE name = '" + name + "'";
		SQLite::Statement query(db, findQuery);
		if (!query.executeStep()) {
			return 0;
		}
		auto id = query.getColumn(0);
		return id;
	} catch (const SQLite::Exception& e) {
		std::cerr << "Select database error: " << e.getErrorStr() << std::endl;
		return 0;
	}
}

Entity::ID AccountHandler::createAccount(const std::string& name, const std::string& password) const
{
	try {
		const auto insertQuery = "INSERT INTO accounts (name, password) "
			"VALUES ('" + name + "', '" + password + "')";
		std::cout << "Query: " << insertQuery << std::endl;
		db.exec(insertQuery);
	} catch (const SQLite::Exception& e) {
		std::cerr << "Insert database error: " << e.getErrorStr() << std::endl;
	}

	return findByName(name);
}

//TODO: Check if it's good idea to return Account - maybe it's better to have some global register?
Account AccountHandler::getEntity(const Entity::ID id) const
{
	Account acc;

	try {
		const auto findQuery = "SELECT * FROM accounts WHERE ID = '" + std::to_string(id) + "'";
		SQLite::Statement query(db, findQuery);
		while (query.executeStep()) {
			acc.setName(query.getColumn("name").getString());
			acc.setPassword(query.getColumn("password").getString());
			acc.setBanned(query.getColumn("banned").getInt());
		}
	}
	catch (const SQLite::Exception& e) {
		std::cerr << "Database error: " << e.getErrorStr() << std::endl;
	}

	return acc;
}

bool AccountHandler::checkName(const std::string& name)
{
	std::cout << "checkName: " << name << std::endl;
	static std::string inv = " \"'~!@#$%^&*+/\\[]{}<>()=.,?;:";

	if (name.find_first_of(inv) != std::string::npos) {
		return false;
	}

	if (name.size() > 16 || name.size() < 3) {
		return false;
	}

	if (!isalpha(name[0])) {
		return false;
	}

	return true;
}

}
