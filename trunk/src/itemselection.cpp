#include <algorithm>
#include <sstream>
#include <iostream>
#include "itemselection.hpp"
#include "utility.hpp"

// TODO: these are not in the header file and in global namespace
bool sortNamed(std::pair<int, Item*> a, std::pair<int, Item*> b)
{
	if (a.second->getType() < b.second->getType()) return true;
	if (a.second->getType() == b.second->getType() && a.first < b.first) return true;
	return false;
}

bool sortAnon(Item* a, Item* b)
{
	if (a->getType() < b->getType()) return true;
	if (a->getType() == b->getType() && a->getName() < b->getName()) return true;
	return false;
}

ItemSelection::ItemSelection(const std::vector<std::pair<int,Item*> >& choices, std::string title, bool mult, bool sort):
	anonymous(false),
	multiple(mult),
	page(0),
	title(title),
	compiledFor(-1)
{
	namedChoices.assign(choices.begin(), choices.end());
	filterTypes.clear();
	if (sort) std::sort(namedChoices.begin(), namedChoices.end(), sortNamed);
}

ItemSelection::ItemSelection(const std::vector<Item*>& choices, std::string title, bool mult, bool sort):
	anonymous(true),
	multiple(mult),
	page(0),
	title(title),
	compiledFor(-1)
{
	anonChoices.assign(choices.begin(), choices.end());
	filterTypes.clear();
	if (sort) std::sort(anonChoices.begin(), anonChoices.end(), sortAnon);
}

std::string ItemSelection::getTitle()
{
	return title;
}

void ItemSelection::setPage(int p)
{
	page = p;
}

int ItemSelection::getPage()
{
	return page;
}

int ItemSelection::getNumPages()
{
	return compiledFor == -1 ? 0 : pageStart.size();
}

void ItemSelection::compile(int height)
{
	// Fix bad heights and check if list is already compiled
	int pageHeight = std::max(3,height);
	if (compiledFor == pageHeight) return;
	
	compiledStrings.clear();
	pageStart.clear();
	pageStart.push_back(0);
	
	int currentPage = 0;
	int currentRow = 0;
	ITEM_TYPE prevType = NUM_ITEM_TYPE;
	
	if (anonymous)
	{
		char currentLetter = 'a';
		for (std::vector<Item*>::iterator it = anonChoices.begin(); it != anonChoices.end(); it++)
		{
			if (currentRow == 0 || (*it)->getType() != prevType || currentRow >= pageHeight) {
				/* Category */
				if (currentRow > 0) currentRow++;
				if (pageHeight - currentRow <= 2)
				{
					currentPage++;
					currentRow = 0;
					currentLetter = 'a';
					pageStart.push_back(compiledStrings.size());
				}
				compiledStrings.push_back(ItemListInfo(currentRow, util::plural((*it)->typeString()), true));
				currentRow += 2;
			}
			/* Item */
			std::stringstream print;
			print << currentLetter << " - " << (*it)->toString();
			compiledStrings.push_back(ItemListInfo(currentRow, print.str(), false));
			
			/* Advance */
			currentRow++;
			prevType = (*it)->getType();
			currentLetter = currentLetter == 'z' ? 'A' : currentLetter + 1;
		}
	}
	else
	{
		for (std::vector<std::pair<int,Item*> >::iterator it = namedChoices.begin(); it != namedChoices.end(); it++)
		{
			if (currentRow == 0 || it->second->getType() != prevType || currentRow >= pageHeight) {
				/* Category */
				if (currentRow > 0) currentRow++;
				if (pageHeight - currentRow <= 2)
				{
					currentPage++;
					currentRow = 0;
					pageStart.push_back(compiledStrings.size());
				}
				compiledStrings.push_back(ItemListInfo(currentRow, util::plural(it->second->typeString()), true));
				currentRow += 2;
			}
			/* Item */
			std::stringstream print;
			print << util::letters[it->first] << " - " << it->second->toString();
			compiledStrings.push_back(ItemListInfo(currentRow, print.str(), false));
			
			/* Advance */
			currentRow++;
			prevType = it->second->getType();
		}
	}
	
	compiledFor = pageHeight;
}

std::vector<ItemListInfo> ItemSelection::getPageList()
{
	std::vector<ItemListInfo> list;
	if (compiledFor == -1) return list;
	page = page % static_cast<int>(pageStart.size());
	int start = pageStart[page];
	int end = static_cast<unsigned int>(page + 1) < pageStart.size() ? pageStart[page + 1] : compiledStrings.size();
	while (start < end)
	{
		list.push_back(compiledStrings[start++]);
	}
	return list;
}

ItemSelection* ItemSelection::filterType(ITEM_TYPE type)
{
	filterTypes.insert(type);
	return this;
}

int ItemSelection::runFilter()
{
	if (anonymous)
	{
		anonChoices.erase(remove_if(anonChoices.begin(), anonChoices.end(),
			std::bind1st(std::mem_fun(&ItemSelection::removeAnonItem), this)), anonChoices.end());
		return anonChoices.size();
	} else {
		namedChoices.erase(remove_if(namedChoices.begin(), namedChoices.end(),
			std::bind1st(std::mem_fun(&ItemSelection::removeNamedItem), this)), namedChoices.end());
		return namedChoices.size();
	}
}

bool ItemSelection::removeAnonItem(Item* item)
{
	return filterTypes.find(item->getType()) == filterTypes.end();
}

bool ItemSelection::removeNamedItem(std::pair<int,Item*> item)
{
	return filterTypes.find(item.second->getType()) == filterTypes.end();
}