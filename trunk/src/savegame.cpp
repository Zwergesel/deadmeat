#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include <sys/stat.h>
#include "savegame.hpp"
#include "world.hpp"
#include "level.hpp"
#include "creature.hpp"
#include "player.hpp"
#include "item.hpp"
#include "items/weapon.hpp"
#include "items/armor.hpp"
#include "items/food.hpp"
#include "items/ammo.hpp"
#include "factory.hpp"

std::string Savegame::version = "0.10";

Savegame::Savegame()
{
	objects = NULL;
}

Savegame::~Savegame()
{
	if (objects != NULL)
	{
		delete[] objects;
		objects = NULL;
	}
}

void Savegame::saveWorld(World& world, std::string fileName)
{
	beginSave(fileName);
	world.save(*this);
	endSave();
}

void Savegame::beginSave(std::string fileName)
{
	// TODO: catch I/O exceptions
	uniqueId = 0;
	obj2id.clear();
	saveStream.open(fileName.c_str());
	writeHeader(0);
}

void Savegame::endSave()
{
	writeHeader(uniqueId);
	saveStream.close();
}

Savegame& operator<<(Savegame& sg, const SaveBlock& sb)
{
	sg.saveStream << std::endl << sb.data.str();
	return sg;
}

bool Savegame::saved(void* voidPtr, unsigned int* voidPtrId)
{
	std::map<void*,unsigned int>::iterator it = obj2id.find(voidPtr);
	if (it == obj2id.end())
	{
		obj2id[voidPtr] = ++uniqueId;
		*voidPtrId = uniqueId;
		return false;
	}
	else
	{
		*voidPtrId = it->second;
		return true;
	}
}

bool Savegame::exists(const std::string& fileName)
{
	struct stat fileInfo;
	return stat(fileName.c_str(), &fileInfo) == 0;
}

void Savegame::deleteSave(const std::string& fileName)
{
	if (exists(fileName))
	{
		if (std::remove(fileName.c_str()) != 0)
		{
			std::cerr << "Could not delete savegame '" << fileName << "' (check write permission)." << std::endl;
		}
	}
}

void Savegame::writeHeader(unsigned int numObjects)
{
	saveStream.seekp(0);
	saveStream << "Version: " << version << std::endl;
	saveStream << "SavedObjects: " << std::setw(8) << std::left << numObjects << std::endl;
}

SaveBlock::SaveBlock(const std::string& header, unsigned int voidPtrId)
{
	data << "[[" << header << ": " << voidPtrId << "]]" << std::endl;
}

SaveBlock& SaveBlock::operator()(const std::string& name, const std::string& input)
{
	data << name << ": " << input << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, int input)
{
	data << name << ": " << input << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, unsigned int input)
{
	data << name << ": " << input << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, double input)
{
	data.precision(32);
	data << name << ": " << input << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, bool input)
{
	data << name << ": " << (input ? "true" : "false") << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, const Point& input)
{
	data << name << ": " << input.x << ", " << input.y << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, const TCODColor& input)
{
	data << name << ": " << static_cast<int>(input.r) << ", ";
	data << static_cast<int>(input.g) << ", " << static_cast<int>(input.b) << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, symbol input)
{
	data << name << ": " << static_cast<int>(input) << std::endl;
	return *this;
}

SaveBlock& SaveBlock::ptr(const std::string& name, unsigned int voidPtrId)
{
	data << name << ": @" << voidPtrId << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, int a, int b, int c)
{
	data << name << ": " << a << ", " << b << ", " << c << std::endl;
	return *this;
}

SaveBlock& SaveBlock::operator()(const std::string& name, Tile* map, int width, int height, bool* seen)
{
	data << name << ":";
	for (int t=0; t<width*height; t++)
	{
		if ((t % width) == 0) data << std::endl;
		data << Savegame::letters.at(static_cast<int>(map[t]));
	}
	data << std::endl << name << ".visibility:";
	for (int t=0; t<width*height; t++)
	{
		if ((t % width) == 0) data << std::endl;
		data << (seen[t] ? '#' : '.');
	}
	data << std::endl;
	return *this;
}

/*--------------------------------------------------------------*/
///////******************** LOADING ***********************///////
/*--------------------------------------------------------------*/

bool Savegame::loadSavegame(std::string fileName)
{
	// TODO: catch I/O exceptions
	loadStream.open(fileName.c_str());

	try
	{
		std::string line;
		unsigned int numObjects;
		loadHeader(line, numObjects);

		// Init pointer structures
		objects = new void*[numObjects+1];
		for (unsigned int x=0; x <= numObjects; x++) objects[x] = NULL;

		while (!loadStream.eof())
		{
			loadObject();
		}
	}
	catch (SavegameFormatException& e)
	{
		std::cerr << "Savegame is corrupt: " << e.what() << std::endl;
		loadStream.close();
		delete[] objects;
		objects = NULL;
		return false;
	}

	// Success
	std::cerr << "Savegame '" << fileName << "' successfully loaded!" << std::endl;
	loadStream.close();
	return true;
}

void Savegame::loadHeader(std::string& v, unsigned int& nObj)
{
	std::string line;
	LoadBlock load(this);
	while (!getline(loadStream, line).eof() && line.length() > 0)
	{
		load << line;
	}
	load ("Version", v) ("SavedObjects", nObj);
}

void Savegame::loadObject()
{
	// Parse header first, format: "[[ClassName: 42]]"
	std::string line;
	if (getline(loadStream,line).eof()) throw SavegameFormatException("loadObject _ unexpected end-of-file");
	if (line.substr(0,2) != "[[" || line.substr(line.length()-2) != "]]")
	{
		throw SavegameFormatException("loadObject _ wrong obj definition [[ ... ]]");
	}
	size_t pos = line.find(": ");
	if (pos == std::string::npos) throw SavegameFormatException("loadObject _ no colon: " + line);
	std::string objClass = line.substr(2,pos-2);
	std::stringstream ss(line.substr(pos+2,line.length()-pos-4));
	unsigned int id;
	if ((ss >> id).fail()) throw SavegameFormatException("loadObject _ integer conversion: " + line);

	// Header ok, load remaining data into a LoadBlock
	LoadBlock load(this);
	while (!getline(loadStream, line).eof() && line.length() > 0)
	{
		load << line;
	}

	// Create object and ask it to load it's data from the LoadBlock
	if (objClass == "World")
	{
		// No new world object. Global world loads this
		objects[id] = static_cast<void*>(&world);
		world.load(load);
	}
	else if (objClass == "Factory")
	{
		// No new Factory object. Global Factory loads this
		objects[id] = static_cast<void*>(&factory);
		factory.load(load);
	}
	else if (objClass == "Creature")
	{
		Creature* obj = new Creature();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Level")
	{
		Level* obj = new Level();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Player")
	{
		Player* obj = new Player();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Item")
	{
		Item* obj = new Item();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Weapon")
	{
		Weapon* obj = new Weapon();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Armor")
	{
		Armor* obj = new Armor();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Food")
	{
		Food* obj = new Food();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Ammo")
	{
		Ammo* obj = new Ammo();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Goblin")
	{
		Goblin* obj = new Goblin();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else if (objClass == "Object")
	{
		Object* obj = new Object();
		objects[id] = static_cast<void*>(obj);
		obj->load(load);
	}
	else
	{
		throw SavegameFormatException("loadObj _ objClass invalid: " + objClass);
	}
	//std::cerr << objClass << " [" << id << "] @ " << objects[id] << std::endl;
}

LoadBlock::LoadBlock(Savegame* sg)
{
	savegame = sg;
}

LoadBlock& operator<<(LoadBlock& lb, const std::string& input)
{
	lb.data << input << std::endl;
	return lb;
}

std::string LoadBlock::parseLine(const std::string& name)
{
	std::string line;
	if (getline(data, line).eof()) throw SavegameFormatException("parseLine _ unexpected end-of-file");
	size_t pos = line.find(": ");
	if (pos == std::string::npos) throw SavegameFormatException("parseLine _ no colon: " + line);
	if (line.substr(0,pos) != name) throw SavegameFormatException("parseLine: " + name + " != " + line.substr(0,pos));
	return line.substr(pos+2);
}

LoadBlock& LoadBlock::operator()(const std::string& name, std::string& output)
{
	output = parseLine(name);
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, int& output)
{
	std::stringstream ss(parseLine(name));
	int result;
	if ((ss >> result).fail())
	{
		throw SavegameFormatException("loadInt _ conversion: " + ss.str());
	}
	output = result;
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, unsigned int& output)
{
	std::stringstream ss(parseLine(name));
	unsigned int result;
	if ((ss >> result).fail())
	{
		throw SavegameFormatException("loadUnsignedInt _ conversion: " + ss.str());
	}
	output = result;
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, double& output)
{
	std::stringstream ss(parseLine(name));
	double result;
	if ((ss >> result).fail())
	{
		throw SavegameFormatException("loadDouble _ conversion: " + ss.str());
	}
	output = result;
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, bool& output)
{
	std::string result = parseLine(name);
	if (result == "true")
	{
		output = true;
	}
	else if (result == "false")
	{
		output = false;
	}
	else
	{
		throw SavegameFormatException("loadBool _ conversion: " + result);
	}
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, Point& output)
{
	std::stringstream ss(parseLine(name));
	char comma;
	if ((ss >> output.x >> comma >> output.y).fail() || comma != ',')
	{
		throw SavegameFormatException("loadPoint _ conversion: " + ss.str());
	}
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, TCODColor& output)
{
	std::stringstream ss(parseLine(name));
	int r,g,b;
	char c1,c2;
	if ((ss >> r >> c1 >> g >> c2 >> b).fail()
	    || c1 != ',' || c2 != ',')
	{
		throw SavegameFormatException("loadColor _ conversion: " + ss.str());
	}
	output = TCODColor(r,g,b);
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, symbol& output)
{
	int result;
	(*this) (name, result);
	output = static_cast<symbol>(result);
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, int& a, int& b, int& c)
{
	std::stringstream ss(parseLine(name));
	int x,y,z;
	char c1,c2;
	if ((ss >> x >> c1 >> y >> c2 >> z).fail()
	    || c1 != ',' || c2 != ',')
	{
		throw SavegameFormatException("loadColor _ conversion: " + ss.str());
	}
	a = x;
	b = y;
	c = z;
	return *this;
}

LoadBlock& LoadBlock::operator()(const std::string& name, Tile* map, int width, int height, bool* seen)
{
	std::string line;
	if (getline(data, line).eof()) throw SavegameFormatException("loadMap _ unexpected end-of-file");
	if (line != name + ":") throw SavegameFormatException("loadMap _ expected name " + name);
	for (int y=0; y<height; y++)
	{
		if (getline(data, line).eof()) throw SavegameFormatException("loadMap _ unexpected end-of-file");
		if (static_cast<int>(line.length()) != width) throw SavegameFormatException("loadMap _ line length does not match map width");
		for (int x=0; x<width; x++)
		{
			size_t t = Savegame::letters.find(line.at(x));
			if (t == std::string::npos || t >= TILES_LENGTH) throw SavegameFormatException("loadMap _ illegal character");
			map[y*width+x] = static_cast<Tile>(t);
		}
	}
	if (getline(data, line).eof()) throw SavegameFormatException("loadMap _ unexpected end-of-file");
	if (line != name + ".visibility:") throw SavegameFormatException("loadMap _ expected name " + name + ".visibility");
	for (int y=0; y<height; y++)
	{
		if (getline(data, line).eof()) throw SavegameFormatException("loadMap _ unexpected end-of-file");
		if (static_cast<int>(line.length()) != width) throw SavegameFormatException("loadMap _ line length does not match map width");
		for (int x=0; x<width; x++)
		{
			seen[y*width+x] = line.at(x) == '#';
		}
	}
	return *this;
}

void* LoadBlock::ptr(const std::string& name)
{
	int id;
	{
		// Forget stringstream when no longer needed
		std::stringstream ss(parseLine(name));
		char at;
		if ((ss >> at >> id).fail() || at != '@')
		{
			throw SavegameFormatException("loadPointer _ conversion: " + ss.str());
		}
	}
	if (id == 0) return NULL;
	while (savegame->objects[id] == NULL)
	{
		if (savegame->loadStream.eof()) throw SavegameFormatException("loadPointer _ unexpected end-of-file");
		savegame->loadObject();
	}
	//std::cerr << "loadPointer @ " << id << ": " << savegame->objects[id] << std::endl;
	return savegame->objects[id];
}

std::string Savegame::letters = "0123456789abcdefghijlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-+#*'_:;!%&/()=?[]{}";