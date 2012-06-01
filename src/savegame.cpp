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
	}
	else
	{
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

void Savegame::loadWorld(std::string fileName)
{
	for (int i=0; i<100; i++) objects[i] = NULL;

	// TODO: catch I/O exceptions
	loadStream.open(fileName.c_str());

	try
	{
		std::string line;
		while (!loadStream.eof())
		{
			loadObject();
		}
	}
	catch (SavegameFormatException& e)
	{
		std::cout << "Savegame is corrupt: " << e.what() << std::endl;
	}

	std::cerr << "Savegame loading complete!" << std::endl;

	loadStream.close();
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
	else
	{
		throw SavegameFormatException("loadObj _ objClass invalid: " + objClass);
	}
	std::cerr << objClass << " [" << id << "] @ " << objects[id] << std::endl;
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
	getline(data, line);
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
		if (savegame->loadStream.eof()) throw SavegameFormatException("loadPointer _ unexpected end-of-file: " + id);
		savegame->loadObject();
	}
	//std::cerr << "loadPointer @ " << id << ": " << savegame->objects[id] << std::endl;
	return savegame->objects[id];
}