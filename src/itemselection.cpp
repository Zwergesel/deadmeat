#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>
#include <libtcod.hpp>
#include "itemselection.hpp"
#include "utility.hpp"
#include "player.hpp"

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

ItemSelection::ItemSelection(): anonymous(true),multiple(false),page(0),title(""),compiled(false),choice(NULL)
{
}

ItemSelection::ItemSelection(const std::vector<std::pair<int,Item*> >& choices, std::string title, bool multiple, bool sort):
	anonymous(false),
	multiple(multiple),
	page(0),
	title(title),
	compiled(false),
	choice(NULL),
	choiceSymbol(0)
{
	namedChoices.assign(choices.begin(), choices.end());
	filterTypes.clear();
	if (sort) std::sort(namedChoices.begin(), namedChoices.end(), sortNamed);
	if (multiple)
	{
		selected.assign(namedChoices.size(), false);
	}
}

ItemSelection::ItemSelection(const std::map<symbol,Item*>& choices, std::string title, bool multiple, bool sort):
	anonymous(false),
	multiple(multiple),
	page(0),
	title(title),
	compiled(false),
	choice(NULL)
{
	std::vector<std::pair<int,Item*> > choices2;
	for (std::map<symbol,Item*>::const_iterator it=choices.begin(); it!=choices.end(); it++)
	{
		choices2.push_back(std::pair<int, Item*>((*it).first, (*it).second));
	}
	namedChoices.assign(choices2.begin(), choices2.end());
	filterTypes.clear();
	if (sort) std::sort(namedChoices.begin(), namedChoices.end(), sortNamed);
	if (multiple)
	{
		selected.assign(namedChoices.size(), false);
	}
}

ItemSelection::ItemSelection(const std::vector<Item*>& choices, std::string title, bool multiple, bool sort):
	anonymous(true),
	multiple(multiple),
	page(0),
	title(title),
	compiled(false),
	choice(NULL),
	choiceSymbol(0)
{
	anonChoices.assign(choices.begin(), choices.end());
	filterTypes.clear();
	if (sort) std::sort(anonChoices.begin(), anonChoices.end(), sortAnon);
	if (multiple)
	{
		selected.assign(anonChoices.size(), false);
	}
}

std::string ItemSelection::getTitle()
{
	return title;
}

int ItemSelection::getNumPages()
{
	return compiled ? pageStart.size() : 0;
}

int ItemSelection::getNumChoices()
{
	return anonymous ? anonChoices.size() : namedChoices.size();
}

Item* ItemSelection::getItem()
{
	assert(!multiple);
	return choice;
}

int ItemSelection::getItemSymbol()
{
	assert(!multiple && !anonymous);
	return choiceSymbol;
}

std::vector<Item*> ItemSelection::getSelection()
{
	assert(multiple);
	std::vector<Item*> list;
	if (anonymous)
	{
		for (unsigned int u = 0; u < anonChoices.size(); u++)
		{
			if (selected[u]) list.push_back(anonChoices[u]);
		}
	}
	else
	{
		for (unsigned int u = 0; u < namedChoices.size(); u++)
		{
			if (selected[u]) list.push_back(namedChoices[u].second);
		}
	}
	return list;
}

std::vector<int> ItemSelection::getSelectionSymbols()
{
	assert(multiple && !anonymous);
	std::vector<int> list;
	for (unsigned int u = 0; u < namedChoices.size(); u++)
	{
		if (selected[u]) list.push_back(namedChoices[u].first);
	}
	return list;
}

ItemSelection* ItemSelection::compile(int height)
{
	// Fix bad heights and check if list is already compiled
	if (compiled) return this;
	int pageHeight = std::max(5,height);

	compiledStrings.clear();
	pageStart.clear();
	pageStart.push_back(0);

	int currentPage = 0;
	int currentRow = 2;
	int currentItem = 0;
	ITEM_TYPE prevType = NUM_ITEM_TYPE;

	if (anonymous)
	{
		char currentLetter = 'a';
		for (std::vector<Item*>::iterator it = anonChoices.begin(); it != anonChoices.end(); it++)
		{
			if (currentRow == 2 || (*it)->getType() != prevType || currentRow >= pageHeight)
			{
				/* Category */
				if (pageHeight - currentRow <= 2)
				{
					currentPage++;
					currentRow = 3;
					currentLetter = 'a';
					pageStart.push_back(compiledStrings.size());
				}
				currentRow++;
				compiledStrings.push_back(CompiledData(currentRow, '#', util::plural((*it)->typeString()), true, -1));
				currentRow += 2;
			}
			/* Item */
			compiledStrings.push_back(CompiledData(currentRow, currentLetter, (*it)->toString(), false, currentItem));

			/* Advance */
			currentRow++;
			currentItem++;
			prevType = (*it)->getType();
			currentLetter = currentLetter == 'z' ? 'A' : currentLetter + 1;
		}
	}
	else
	{
		for (std::vector<std::pair<int,Item*> >::iterator it = namedChoices.begin(); it != namedChoices.end(); it++)
		{
			if (currentRow == 2 || it->second->getType() != prevType || currentRow >= pageHeight)
			{
				/* Category */
				if (pageHeight - currentRow <= 2)
				{
					currentPage++;
					currentRow = 2;
					pageStart.push_back(compiledStrings.size());
				}
				currentRow++;
				compiledStrings.push_back(CompiledData(currentRow, '#', util::plural(it->second->typeString()), true, -1));
				currentRow += 2;
			}
			/* Item */
			compiledStrings.push_back(CompiledData(currentRow, util::letters[it->first],
			                                       it->second->toString(), false, currentItem));

			/* Advance */
			currentRow++;
			currentItem++;
			prevType = it->second->getType();
		}
	}

	compiled = true;
	return this;
}

void ItemSelection::resetDraw()
{
	if (compiled && page < static_cast<int>(pageStart.size())) drawCounter = pageStart[page];
}

bool ItemSelection::hasDrawLine()
{
	int end = static_cast<unsigned int>(page + 1) < pageStart.size() ? pageStart[page + 1] : compiledStrings.size();
	return (compiled && drawCounter < end);
}

std::string ItemSelection::getNextLine(int* row, bool* category)
{
	CompiledData dat = compiledStrings[drawCounter++];
	*category = dat.category;
	*row = dat.row;
	if (dat.category) return dat.text;
	std::stringstream print;
	print << dat.letter << ((multiple && selected[dat.itemIndex]) ? " + " : " - ") << dat.text;
	return print.str();
}

/* Return true means the selection has been made */
bool ItemSelection::keyInput(TCOD_key_t key)
{
	if (!compiled) return true;
	if (!key.pressed) return false;
	if (key.vk == TCODK_ESCAPE)
	{
		// Deselect all and end
		if (multiple)
		{
			selected.assign(anonymous ? anonChoices.size() : namedChoices.size(), false);
		}
		else
		{
			choice = NULL;
			choiceSymbol = -1;
		}
		return true;
	}
	else if (key.vk == TCODK_SPACE)
	{
		// Advance one page, quit if the end was reached
		if (++page >= static_cast<int>(pageStart.size())) return true;
	}
	else if (multiple && (key.c == ',' || key.c == '+'))
	{
		// Select all on current page
		selectAllOnPage(true);
		return false;
	}
	else if (multiple && key.c == '-')
	{
		// Deselect all on current page
		selectAllOnPage(false);
		return false;
	}
	else if ((key.c >= 'a' && key.c <= 'z') || (key.c >= 'A' && key.c <= 'Z'))
	{
		// Select an item
		return toggleItem(key.c);
	}
	else if (key.c == '>')
	{
		// Advance one page
		if (page + 1 < static_cast<int>(pageStart.size())) page++;
		return false;
	}
	else if (key.c == '<')
	{
		// Go back one page
		if (page > 0) page--;
		return false;
	}
	return false;
}

bool ItemSelection::toggleItem(char c)
{
	if (page >= static_cast<int>(pageStart.size())) return false;
	unsigned int start = pageStart[page];
	unsigned int end = static_cast<unsigned int>(page + 1) < pageStart.size() ? pageStart[page + 1] : compiledStrings.size();
	while (start < end)
	{
		CompiledData& info = compiledStrings[start++];
		if (info.letter == c)
		{
			if (multiple)
			{
				selected[info.itemIndex] = !selected[info.itemIndex];
				return false;
			}
			else if (anonymous)
			{
				choice = anonChoices[info.itemIndex];
				return true;
			}
			else
			{
				choice = namedChoices[info.itemIndex].second;
				choiceSymbol = namedChoices[info.itemIndex].first;
				return true;
			}
		}
	}
	return false;
}

void ItemSelection::selectAllOnPage(bool set)
{
	if (!multiple || page >= static_cast<int>(pageStart.size())) return;
	unsigned int start = pageStart[page];
	unsigned int end = static_cast<unsigned int>(page + 1) < pageStart.size() ? pageStart[page + 1] : compiledStrings.size();
	while (start < end)
	{
		CompiledData info = compiledStrings[start++];
		if (!info.category) selected[info.itemIndex] = set;
	}
}

ItemSelection* ItemSelection::filterType(ITEM_TYPE type)
{
	filterTypes.insert(type);
	return this;
}

ItemSelection* ItemSelection::runFilter()
{
	if (anonymous)
	{
		anonChoices.erase(remove_if(anonChoices.begin(), anonChoices.end(),
		                            std::bind1st(std::mem_fun(&ItemSelection::removeAnonItem), this)), anonChoices.end());
		if (multiple) selected.assign(anonChoices.size(), false);
	}
	else
	{
		namedChoices.erase(remove_if(namedChoices.begin(), namedChoices.end(),
		                             std::bind1st(std::mem_fun(&ItemSelection::removeNamedItem), this)), namedChoices.end());
		if (multiple) selected.assign(namedChoices.size(), false);
	}
	return this;
}

bool ItemSelection::removeAnonItem(Item* item)
{
	return filterTypes.find(item->getType()) == filterTypes.end();
}

bool ItemSelection::removeNamedItem(std::pair<int,Item*> item)
{
	return filterTypes.find(item.second->getType()) == filterTypes.end();
}