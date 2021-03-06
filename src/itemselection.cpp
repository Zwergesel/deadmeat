#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>
#include <libtcod.hpp>
#include "itemselection.hpp"
#include "utility.hpp"
#include "player.hpp"

// TODO: these are not in the header file and in global namespace
bool sortNamed(std::pair<symbol, Item*> a, std::pair<symbol, Item*> b)
{
	if (a.second->getType() < b.second->getType()) return true;
	if (a.second->getType() == b.second->getType()) return a.first < b.first;
	return false;
}

bool sortAnon(Item* a, Item* b)
{
	if (a->getType() < b->getType()) return true;
	if (a->getType() == b->getType() && a->toString() < b->toString()) return true;
	return false;
}

ItemSelection::ItemSelection(): anonymous(true),multiple(false),page(0),title(""),compiled(false),splitAmount(0),choice(NULL)
{
}

ItemSelection::ItemSelection(const std::vector<std::pair<symbol,Item*> >& choices, std::string title, bool multiple, bool sort):
	anonymous(false),
	multiple(multiple),
	page(0),
	title(title),
	compiled(false),
	splitAmount(0),
	choice(NULL),
	choiceSymbol('\0')
{
	namedChoices.assign(choices.begin(), choices.end());
	filterTypes.clear();
	if (sort) std::sort(namedChoices.begin(), namedChoices.end(), sortNamed);
	if (multiple)
	{
		selected.assign(namedChoices.size(), 0);
	}
}

ItemSelection::ItemSelection(const std::map<symbol,Item*>& choices, std::string title, bool multiple, bool sort):
	anonymous(false),
	multiple(multiple),
	page(0),
	title(title),
	compiled(false),
	splitAmount(0),
	choice(NULL),
	choiceSymbol('\0')
{
	for (std::map<symbol,Item*>::const_iterator it = choices.begin(); it != choices.end(); it++)
	{
		namedChoices.push_back(*it);
	}
	filterTypes.clear();
	if (sort) std::sort(namedChoices.begin(), namedChoices.end(), sortNamed);
	if (multiple)
	{
		selected.assign(namedChoices.size(), 0);
	}
}

ItemSelection::ItemSelection(const std::vector<Item*>& choices, std::string title, bool multiple, bool sort):
	anonymous(true),
	multiple(multiple),
	page(0),
	title(title),
	compiled(false),
	splitAmount(0),
	choice(NULL),
	choiceSymbol('\0')
{
	anonChoices.assign(choices.begin(), choices.end());
	filterTypes.clear();
	if (sort) std::sort(anonChoices.begin(), anonChoices.end(), sortAnon);
	if (multiple)
	{
		selected.assign(anonChoices.size(), 0);
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

symbol ItemSelection::getItemSymbol()
{
	assert(!multiple && !anonymous);
	return choiceSymbol;
}

std::vector<std::pair<Item*,int>> ItemSelection::getSelection()
{
	assert(multiple);
	std::vector<std::pair<Item*,int>> list;
	if (anonymous)
	{
		for (unsigned int u = 0; u < anonChoices.size(); u++)
		{
			if (selected[u] > 0) list.push_back(std::make_pair(anonChoices[u], selected[u]));
		}
	}
	else
	{
		for (unsigned int u = 0; u < namedChoices.size(); u++)
		{
			if (selected[u] > 0) list.push_back(std::make_pair(namedChoices[u].second, selected[u]));
		}
	}
	return list;
}

std::vector<std::pair<symbol,int>> ItemSelection::getSelectionSymbols()
{
	assert(multiple && !anonymous);
	std::vector<std::pair<symbol,int>> list;
	for (unsigned int u = 0; u < namedChoices.size(); u++)
	{
		if (selected[u] > 0) list.push_back(std::make_pair(namedChoices[u].first, selected[u]));
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
		symbol currentLetter = 'a';
		for (std::vector<Item*>::iterator it = anonChoices.begin(); it != anonChoices.end(); it++)
		{
			if (currentRow == 2 || (*it)->getType() != prevType || currentRow >= pageHeight - 3)
			{
				/* Category */
				if (currentRow >= pageHeight - 5)
				{
					currentPage++;
					currentRow = 3;
					currentLetter = 'a';
					pageStart.push_back(compiledStrings.size());
				}
				currentRow++;
				compiledStrings.push_back(CompiledData(currentRow, '\0', util::capitalize((*it)->typeString()), true, -1));
				currentRow += 2;
			}
			/* Item */
			compiledStrings.push_back(CompiledData(currentRow, currentLetter, util::format(FORMAT_INDEF, *it), false, currentItem));

			/* Advance */
			currentRow++;
			currentItem++;
			prevType = (*it)->getType();
			currentLetter = currentLetter == 'z' ? 'A' : currentLetter + 1;
		}
	}
	else
	{
		for (std::vector<std::pair<symbol,Item*> >::iterator it = namedChoices.begin(); it != namedChoices.end(); it++)
		{
			if (currentRow == 2 || it->second->getType() != prevType || currentRow >= pageHeight - 3)
			{
				/* Category */
				if (currentRow >= pageHeight - 5)
				{
					currentPage++;
					currentRow = 2;
					pageStart.push_back(compiledStrings.size());
				}
				currentRow++;
				compiledStrings.push_back(CompiledData(currentRow, '\0', util::capitalize(it->second->typeString()), true, -1));
				currentRow += 2;
			}
			/* Item */
			compiledStrings.push_back(CompiledData(currentRow, it->first, util::format(FORMAT_INDEF, it->second), false, currentItem));

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
	print << dat.letter;
	if (multiple && selected[dat.itemIndex] > 0)
	{
		int maxAmount = anonymous ? anonChoices[dat.itemIndex]->getAmount() : namedChoices[dat.itemIndex].second->getAmount();
		print << (selected[dat.itemIndex] == maxAmount ? " + " : " # ");
	}
	else
	{
		print << " - ";
	}
	print << dat.text;
	return print.str();
}

/* Return true means the selection has been made */
bool ItemSelection::keyInput(TCOD_key_t key)
{
	assert(compiled);
	if (!key.pressed) return false;

	// Other keys
	if (key.vk == TCODK_ESCAPE)
	{
		// Deselect all and end
		if (multiple)
		{
			selected.assign(anonymous ? anonChoices.size() : namedChoices.size(), 0);
		}
		else
		{
			choice = NULL;
			choiceSymbol = '\0';
		}
		return true;
	}
	else if (key.vk == TCODK_ENTER)
	{
		return true;
	}
	else if (key.vk == TCODK_SPACE)
	{
		// Advance one page, quit if the end was reached
		splitAmount = 0;
		if (++page >= static_cast<int>(pageStart.size())) return true;
	}
	else if (key.c >= '0' && key.c <= '9')
	{
		// Change split amount
		int digit = key.c - '0';
		splitAmount = 10*splitAmount + digit;
		return false;
	}
	else if (multiple && (key.c == ',' || key.c == '+'))
	{
		// Select all on current page
		selectAllOnPage(true);
		splitAmount = 0;
		return false;
	}
	else if (multiple && key.c == '-')
	{
		// Deselect all on current page
		selectAllOnPage(false);
		splitAmount = 0;
		return false;
	}
	else if (key.c == '>')
	{
		// Advance one page
		if (page + 1 < static_cast<int>(pageStart.size())) page++;
		splitAmount = 0;
		return false;
	}
	else if (key.c == '<')
	{
		// Go back one page
		if (page > 0) page--;
		splitAmount = 0;
		return false;
	}
	else if ((key.c >= 'a' && key.c <= 'z') || (key.c >= 'A' && key.c <= 'Z') || key.c == '*' || key.c == '$')
	{
		// Select an item
		bool exit = toggleItem(key.c);
		splitAmount = 0;
		return exit;
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
			int maxAmount = anonymous ? anonChoices[info.itemIndex]->getAmount() : namedChoices[info.itemIndex].second->getAmount();
			if (multiple && splitAmount > 0)
			{
				selected[info.itemIndex] = std::min(splitAmount, maxAmount);
				return false;
			}
			else if (multiple)
			{
				selected[info.itemIndex] = selected[info.itemIndex] < maxAmount ? maxAmount : 0;
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
		if (info.category) continue;
		if (set)
		{
			int maxAmount = anonymous ? anonChoices[info.itemIndex]->getAmount() : namedChoices[info.itemIndex].second->getAmount();
			selected[info.itemIndex] = maxAmount;
		}
		else
		{
			selected[info.itemIndex] = 0;
		}
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
		if (multiple) selected.assign(anonChoices.size(), 0);
	}
	else
	{
		namedChoices.erase(remove_if(namedChoices.begin(), namedChoices.end(),
		                             std::bind1st(std::mem_fun(&ItemSelection::removeNamedItem), this)), namedChoices.end());
		if (multiple) selected.assign(namedChoices.size(), 0);
	}
	return this;
}

bool ItemSelection::removeAnonItem(Item* item)
{
	return filterTypes.find(item->getType()) == filterTypes.end();
}

bool ItemSelection::removeNamedItem(std::pair<symbol,Item*> item)
{
	return filterTypes.find(item.second->getType()) == filterTypes.end();
}