#include "fileparser.hpp"
#include "factory.hpp"
#include "item.hpp"
#include <vector>
#include <fstream>

void FileParser::loadDestructionTable()
{
	std::ifstream file;
	file.open("data/destroy.cfg");
	if (!file.is_open()) return;
	std::string line, itemClass = "none-item";
	while (!file.eof())
	{
		getline(file,line);
		if (line.size() == 0) continue;
		if (line[0] == '#') continue;
		if (line[0] == '[' && line[line.length()-1] == ']')
		{
			itemClass = line.substr(1, line.length()-2);
		}
		else
		{
			if (Item::DestructionTable.find(itemClass) == Item::DestructionTable.end())
			{
				Item::DestructionTable[itemClass] = std::vector<std::string>();
			}
			Item::DestructionTable[itemClass].push_back(line);
		}
	}
	file.close();
}

std::vector<std::string> FileParser::loadStartItems(PlayerClass c, PlayerRace r, Gender g)
{
	std::vector<std::string> items;
	std::ifstream itemFile;
	itemFile.open("data/chargen.cfg");
	if (!itemFile.is_open()) return items;
	std::string line;
	bool discard = true;
	while (!itemFile.eof())
	{
		getline(itemFile,line);
		if (line.size() == 0) continue;
		if (line[0] == '#') continue;
		if (line[0] == '[')
		{
			size_t split = line.find_first_of(' ');
			size_t end = line.find_first_of(']');
			std::string type = line.substr(1, split-1);
			std::string compare = line.substr(split+1, end-split-1);
			if (type == "class" && compare == CharGen::CLASS_NAMES[c]) discard = false;
			else if (type == "race" && compare == CharGen::RACE_NAMES[r]) discard = false;
			else discard = true;
		}
		else if (!discard)
		{
			items.push_back(line);
		}
	}
	itemFile.close();
	return items;
}

Item* FileParser::getItemFromString(const std::string& info)
{
	size_t pos = 0;
	std::string itemClass = "";
	int number(0), dice(0), total(0), sign(1);
	bool expectDice(false), expectString(true);
	
	// This understands syntax like "5d4-1d6+12 arrow"
	while (pos < info.length())
	{
		if (info[pos] == ' ') {
			expectString = true;
		}
		else if (info[pos] >= '0' && info[pos] <= '9')
		{
			number = 10 * number + (info[pos] - '0');
			expectString = false;
		}
		else if (expectDice)
		{
			while (--dice >= 0)
			{
				total += sign * rng->getInt(1,number);
			}
			expectDice = false;
			number = 0;
		}
		else if (!expectString && info[pos] == 'd')
		{
			expectDice = true;
			dice = number;
			number = 0;
		}
		else
		{
			total += sign * number;
			number = 0;
		}
		
		if (info[pos] == '+' || info[pos] == '-')
		{
			sign = info[pos] == '-' ? -1 : 1;
		}
		else if (expectString && info[pos] != ' ')
		{		
			itemClass = info.substr(pos);
			break;
		}
		pos++;
	}
	
	if (total > 0 && factory.itemExists(itemClass))
	{
		Item* item = factory.spawnItem(itemClass, true);
		item->setAmount(total);
		return item;
	}
	else
	{
		return NULL;
	}
}