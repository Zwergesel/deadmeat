#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "savegame.hpp"
#include "world.hpp"
#include "level.hpp"
#include "creature.hpp"
#include "player.hpp"
#include "item.hpp"
#include "items/weapon.hpp"
#include "items/armor.hpp"

Savegame::Savegame()
{
}

Savegame::~Savegame()
{
	std::cerr << "~Savegame() @ " << this << std::endl;
}

void Savegame::saveWorld(World& world, std::string fileName)
{
	uniqueId = 0;
	firstBlock = true;
	
	// TODO: catch I/O exceptions
	saveStream.open(fileName.c_str());
	world.save(*this);
	saveStream.close();
}

Savegame& operator<<(Savegame& sg, const SaveBlock& sb)
{
	if (!sg.firstBlock) sg.saveStream << std::endl;
	sg.saveStream << sb.data.str();
	sg.firstBlock = false;
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
	} else {
		*voidPtrId = it->second;
		return true;
	}
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

SaveBlock& SaveBlock::ptr(const std::string& name, unsigned int voidPtrId)
{
	data << name << ": @" << voidPtrId << std::endl;
	return *this;
}

/*--------------------------------------------------------------*/
///////******************** LOADING ***********************///////
/*--------------------------------------------------------------*/

World* Savegame::loadWorld(std::string fileName)
{
	for (int i=0; i<100; i++) objects[i] = NULL;

	// TODO: catch I/O exceptions
	loadStream.open(fileName.c_str());
	
	try
	{
		std::string line;
		while (!loadStream.eof())
		{
			loadBlock();
		}
	}
	catch(SavegameFormatException& e)
	{
		std::cout << "Savegame is corrupt: " << e.what() << std::endl;
	}
	
	loadStream.close();
	return NULL;
}

void Savegame::loadBlock()
{
	std::stringstream ss;
	std::string line;
	while (!getline(loadStream, line).eof() && line.length() > 0)
	{
		ss << line << std::endl;
	}
	loadObj(ss);
}

void Savegame::loadObj(std::stringstream& is)
{
	std::string line;
	getline(is,line);
	if (line.substr(0,2) != "[[" || line.substr(line.length()-2) != "]]")
	{
		throw SavegameFormatException("loadWorld _ wrong obj definition [[ ... ]]");
	}
	size_t pos = line.find(": ");
	if (pos == std::string::npos) throw SavegameFormatException("loadObj _ no colon: " + line);
	std::string objClass = line.substr(2,pos-2);
	std::stringstream ss(line.substr(pos+2,line.length()-pos-4));
	unsigned int id;
	if ((ss >> id).fail()) throw SavegameFormatException("loadObj _ integer conversion: " + line);
	if (objClass == "World")
	{
		world = *(new World());
		objects[id] = static_cast<void*>(&world);
		world.load(this, is);
	}
	else if (objClass == "Creature")
	{
		Creature* obj = new Creature();
		objects[id] = static_cast<void*>(obj);
		obj->load(this, is);
	}
	else if (objClass == "Level")
	{
		Level* obj = new Level();
		objects[id] = static_cast<void*>(obj);
		obj->load(this, is);
	}
	else if (objClass == "Player")
	{
		Player* obj = new Player();
		objects[id] = static_cast<void*>(obj);
		obj->load(this, is);
	}
	else if (objClass == "Item")
	{
		Item* obj = new Item();
		objects[id] = static_cast<void*>(obj);
		obj->load(this, is);
	}
	else if (objClass == "Weapon")
	{
		Weapon* obj = new Weapon();
		objects[id] = static_cast<void*>(obj);
		obj->load(this, is);
	}
	else if (objClass == "Armor")
	{
		Armor* obj = new Armor();
		objects[id] = static_cast<void*>(obj);
		obj->load(this, is);
	}
	else
	{
		throw SavegameFormatException("loadObj _ objClass invalid: " + objClass);
	}
	std::cerr << objClass << " [" << id << "] @ " << objects[id] << std::endl;
}

std::string Savegame::parseLine(const std::string& name, std::stringstream& is)
{
	std::string line;
	getline(is, line);
	size_t pos = line.find(": ");
	if (pos == std::string::npos) throw SavegameFormatException("parseLine _ no colon: " + line);
	if (line.substr(0,pos) != name) throw SavegameFormatException("parseLine: " + name + " != " + line.substr(0,pos));
	return line.substr(pos+2);
}

std::string Savegame::loadString(std::string name, std::stringstream& is)
{
	return parseLine(name, is);
}

int Savegame::loadInt(std::string name, std::stringstream& is)
{
	std::stringstream ss(parseLine(name, is));
	int result;
	if ((ss >> result).fail())
	{
		throw SavegameFormatException("loadInt _ conversion: " + ss.str());
	}
	return result;
}

double Savegame::loadDouble(std::string name, std::stringstream& is)
{
	std::stringstream ss(parseLine(name, is));
	double result;
	if ((ss >> result).fail())
	{
		throw SavegameFormatException("loadDouble _ conversion: " + ss.str());
	}
	return result;
}

bool Savegame::loadBool(std::string name, std::stringstream& is)
{
	std::string result = parseLine(name, is);
	if (result == "true")
	{
		return true;
	}
	else if (result != "false")
	{
		throw SavegameFormatException("loadBool _ conversion: " + result);
	}
	return false;
}

Point Savegame::loadPoint(std::string name, std::stringstream& is)
{
	std::stringstream ss(parseLine(name, is));
	Point p;
	char comma;
	if ((ss >> p.x >> comma >> p.y).fail() || comma != ',')
	{
		throw SavegameFormatException("loadPoint _ conversion: " + ss.str());
	}
	return p;
}

TCODColor Savegame::loadColor(std::string name, std::stringstream& is)
{
	std::stringstream ss(parseLine(name, is));
	int r,g,b;
	char c1,c2;
	if ((ss >> r >> c1 >> g >> c2 >> b).fail()
		|| c1 != ',' || c2 != ',')
	{
		throw SavegameFormatException("loadColor _ conversion: " + ss.str());
	}
	return TCODColor(r,g,b);
}

Viewport Savegame::loadViewport(std::string name, std::stringstream& is)
{
	std::stringstream ss(parseLine(name, is));
	Viewport v;
	char c1,c2,c3;
	if ((ss >> v.x >> c1 >> v.y >> c2 >> v.width >> c3 >> v.height).fail()
		|| c1 != ',' || c2 != ',' || c3 != ',')
	{
		throw SavegameFormatException("loadPoint _ conversion: " + ss.str());
	}
	return v;
}

void* Savegame::loadPointer(std::string name, std::stringstream& is)
{
	int id = loadInt(name, is);
	if (id == 0) return NULL;
	while (objects[id] == NULL) {
		if (loadStream.eof()) throw SavegameFormatException("loadPointer _ unexpected end-of-file: " + id);
		loadBlock();
	}
	//std::cerr << "loadPointer @ " << id << ": " << objects[id] << std::endl;
	return objects[id];
}