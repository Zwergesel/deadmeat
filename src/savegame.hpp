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
	SavegameFormatException():detail("???"){};
	SavegameFormatException(std::string s):detail(s){};
	~SavegameFormatException() throw() {};
	virtual const char* what() const throw()
	{
		return detail.c_str();
	}
};

class SaveBlock
{
private:
public:
	std::stringstream data;
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

class Savegame
{
private:
	unsigned int uniqueId;
	std::map<void*,unsigned int> obj2id;
	void* objects[100];
	std::ofstream saveStream;
	std::ifstream loadStream;
	bool firstBlock;
	friend Savegame& operator<<(Savegame& sg, const SaveBlock& sb);

public:
	Savegame();
	~Savegame();
	
	bool saved(void* voidPtr, unsigned int* voidPtrId);
	void saveWorld(World& world, std::string fileName);
	
	World* loadWorld(std::string fileName);
	void loadBlock();
	void loadObj(std::stringstream& ss);
	std::string parseLine(const std::string& name, std::stringstream& ss);
	std::string loadString(std::string name, std::stringstream& ss);
	int loadInt(std::string name, std::stringstream& ss);
	double loadDouble(std::string name, std::stringstream& ss);
	bool loadBool(std::string name, std::stringstream& ss);
	Point loadPoint(std::string name, std::stringstream& ss);
	TCODColor loadColor(std::string name, std::stringstream& ss);
	Viewport loadViewport(std::string name, std::stringstream& ss);
	void* loadPointer(std::string name, std::stringstream& ss);
};

#endif