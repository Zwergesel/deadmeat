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

class Savegame
{
private:
	unsigned int uniqueId;
	std::map<void*,unsigned int> obj2id;
	void* objects[100];
	std::ofstream saveStream;
	std::ifstream loadStream;

public:
	Savegame();
	~Savegame();
	
	void flushStringstream(std::stringstream& ss);
	bool objExists(void* obj);
	unsigned int objId(void* obj);
	
	void saveWorld(World* world, std::string fileName);
	void saveObj(void* obj, std::string name, std::stringstream& ss);
	void saveString(std::string s, std::string name, std::stringstream& ss);
	void saveInt(int i, std::string name, std::stringstream& ss);
	void saveDouble(double d, std::string name, std::stringstream& ss);
	void saveBool(bool b, std::string name, std::stringstream& ss);
	void savePoint(Point p, std::string name, std::stringstream& ss);
	void saveColor(TCODColor& c, std::string name, std::stringstream& ss);
	void saveViewport(Viewport v, std::string name, std::stringstream& ss);
	void savePointer(unsigned int id, std::string name, std::stringstream& ss);
	
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