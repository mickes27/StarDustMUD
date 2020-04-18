#pragma once
#include <SQLiteCpp/Database.h>
#include "../../Entities/Entity.h"

namespace GwiezdnyPyl {
	
class DatabaseHandler
{
public:
	DatabaseHandler(SQLite::Database& db) :
		db(db)
	{}
	virtual ~DatabaseHandler() = default;

protected:
	SQLite::Database& db;
};

}