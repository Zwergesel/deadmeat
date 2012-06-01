#ifndef _SAVEGAME_HPP
#define _SAVEGAME_HPP

/* Includes */
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include <libtcod.hpp>
#include <map>
#include "utility.hpp"

/* Forward declarations */
class World;
class Level;
class Creature;
class Player;
struct Viewport;
class Item;
class Weapon;
class Armor;
class Savegame;

/* Actual classes */
class SavegameFormatException: public std::exception
{
private:
	std::string detail;
public:
	SavegameFormatException(std::string s):detail(s) {};
	~SavegameFormatException() throw() {};
	virtual const char* what() const throw()
	{
		return detail.c_str();
	}
};

class SaveBlock
{
private:
	std::stringstream data;
public:
	SaveBlock(const std::string& header, unsigned int voidPtrId);
	SaveBlock& operator()(const std::string& name, const std::string& input);
	SaveBlock& operator()(const std::string& name, int input);
	SaveBlock& operator()(const std::string& name, double input);
	SaveBlock& operator()(const std::string& name, bool input);
	SaveBlock& operator()(const std::string& name, const Point& input);
	SaveBlock& operator()(const std::string& name, const TCODColor& input);
	SaveBlock& ptr(const std::string& name, unsigned int voidPtrId);
	friend Savegame& operator<<(Savegame& sg, const SaveBlock& sb);
};

class LoadBlock
{
private:
	std::stringstream data;
	Savegame* savegame;
	std::string parseLine(const std::string& name);
	friend LoadBlock& operator<<(LoadBlock& lb, const std::string& input);
public:
	LoadBlock(Savegame*);
	LoadBlock& operator()(const std::string& name, std::string& output);
	LoadBlock& operator()(const std::string& name, int& output);
	LoadBlock& operator()(const std::string& name, double& output);
	LoadBlock& operator()(const std::string& name, bool& output);
	LoadBlock& operator()(const std::string& name, Point& output);
	LoadBlock& operator()(const std::string& name, TCODColor& output);
	void* ptr(const std::string& name);
};

class Savegame
{
private:
	unsigned int uniqueId;
	std::map<void*,unsigned int> obj2id;
	void* objects[100];
	std::ofstream saveStream;
	std::ifstream loadStream;
	bool firstBlock;
	void loadObject();
	friend Savegame& operator<<(Savegame& sg, const SaveBlock& sb);
	friend void* LoadBlock::ptr(const std::string& name);

public:
	Savegame();
	~Savegame();

	bool saved(void* voidPtr, unsigned int* voidPtrId);
	void saveWorld(World& world, std::string fileName);
	void loadWorld(std::string fileName);
};

#endif