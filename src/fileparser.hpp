#ifndef _FILEPARSER_HPP
#define _FILEPARSER_HPP

#include <string>
#include <algorithm>
#include <libtcod.hpp>

class Item;

namespace FileParser
{	
	void loadDestructionTable();
	Item* getItemFromString(const std::string& info);
}

#endif
