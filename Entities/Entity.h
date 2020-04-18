#pragma once
#include <string>
#include <set>

namespace GwiezdnyPyl {

class Entity
{
public:
	using ID = signed int;
	using handler = std::shared_ptr<Entity>;

	Entity() :
		id(0),
		name("Unknown"),
		description("Unknown")
	{}

	ID getID() const { return id; }
	std::string getName() const { return name; }
	std::string getDesciption() const { return description; }

	void setID(const ID id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setDescription(const std::string& description) { this->description = description; }

private:
	ID id;
	std::string name;
	std::string description;
};

class InArea
{
public:
	InArea() : 
		area(0) 
	{}

	Entity::ID getArea() const { return area; }
	void setArea(const Entity::ID area) { this->area = area; }

protected:
	Entity::ID area;
};

class InRoom
{
public:
	InRoom() :
		room(0) 
	{}

	Entity::ID getRoom() const { return room; }
	void setRoom(const Entity::ID room) { this->room = room; }

protected:
	Entity::ID room;
};

class HasCharacters
{
public:
	using Characters = std::set<Entity::ID>;
	void addCharacter(const Entity::ID id) { characters.insert(id); }
	void delCharacter(const Entity::ID id) { characters.erase(id); }
	auto charactersBegin() { return characters.begin(); }
	auto charactersEnd() { return characters.end(); }
	size_t getCharacters() const { return characters.size(); }
protected:
	Characters characters;
};

class HasItems
{
public:
	using Items = std::set<Entity::ID>;
	void addItem(const Entity::ID id) { items.insert(id); }
	void delItem(const Entity::ID id) { items.erase(id); }
	auto itemsBegin() { return items.begin(); }
	auto itemsEnd() { return items.end(); }
	size_t getItems() const { return items.size(); }
protected:
	Items items;
};

class HasRooms
{
public:
	using Rooms = std::set<Entity::ID>;
	void addRoom(const Entity::ID id) { rooms.insert(id); }
	void delRoom(const Entity::ID id) { rooms.erase(id); }
	auto roomsBegin() { return rooms.begin(); }
	auto roomsEnd() { return rooms.end(); }
	size_t getRooms() const { return rooms.size(); }
protected:
	Rooms rooms;
};

class HasPortals
{
public:
	using Portals = std::set<Entity::ID>;
	void addPortal(const Entity::ID id) { portals.insert(id); }
	void delPortal(const Entity::ID id) { portals.erase(id); }
	auto portalsBegin() { return portals.begin(); }
	auto portalsEnd() { return portals.end(); }
	size_t getPortals() const { return portals.size(); }
protected:
	Portals portals;
};

}
