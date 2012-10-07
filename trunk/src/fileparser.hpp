#ifndef _FILEPARSER_HPP
#define _FILEPARSER_HPP

#include <string>
#include <algorithm>
#include <libtcod.hpp>
#include <vector>
#include <map>
#include "chargen.hpp"

class Item;

namespace FileParser
{	
	void loadDestructionTable(std::map<std::string, std::vector<std::string>>& table);
	std::vector<std::string> loadStartItems(PlayerClass c, PlayerRace r, Gender g);
	Item* getItemFromString(const std::string& info);
}

#endif
