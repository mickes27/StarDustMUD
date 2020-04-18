#pragma once
#include "Entity.h"


namespace GwiezdnyPyl {

class Account : public Entity,
				public HasCharacters
{
public:
	Account() :
		password(""),
		banned(false)
	{}

	std::string getPassword() const { return password; }
	bool isBanned() const { return banned; }

	void setPassword(const std::string& password) { this->password = password; }
	void setBanned(const bool banned) { this->banned = banned; }

private:
	std::string password;
	bool banned;
};

}