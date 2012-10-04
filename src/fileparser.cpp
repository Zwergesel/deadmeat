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

Item* FileParser::getItemFromString(const std::string& info)
{
	size_t pos = 0;
	std::string itemClass = "";
	int number(0), dice(0), total(0), sign(1);
	bool expectDice(false), expectString(true);
	
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