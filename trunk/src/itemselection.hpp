#ifndef _ITEMSELECTION_HPP
#define _ITEMSELECTION_HPP

#include <vector>
#include <map>
#include <set>
#include <string>
#include <libtcod.hpp>
#include "item.hpp"

class Player;

struct CompiledData
{
	int row;
	char letter;
	std::string text;
	bool category;
	int itemIndex;
	CompiledData(int r, char l, std::string t, bool c, int i):row(r),letter(l),text(t),category(c),itemIndex(i) {};
};

class ItemSelection
{

private:
	std::vector<std::pair<int,Item*> > namedChoices;
	std::vector<Item*> anonChoices;
	bool anonymous;
	bool multiple;
	int page;
	std::string title;
	std::set<ITEM_TYPE> filterTypes;

	bool compiled;
	std::vector<CompiledData> compiledStrings;
	std::vector<int> pageStart;
	std::vector<bool> selected;
	Item* choice;
	int choiceSymbol;
	int drawCounter;

	bool removeAnonItem(Item* item);
	bool removeNamedItem(std::pair<int,Item*> item);
	bool toggleItem(char c);
	void selectAllOnPage(bool set);

public:
	ItemSelection();
	ItemSelection(const std::vector<Item*>& choices, std::string title, bool multiple = false, bool sort = true);
	ItemSelection(const std::vector<std::pair<int,Item*> >& choices, std::string title, bool multiple = false, bool sort = true);
	ItemSelection(const std::map<symbol,Item*>& choices, std::string title, bool multiple = false, bool sort = true);

	ItemSelection* filterType(ITEM_TYPE type);
	ItemSelection* runFilter();
	ItemSelection* compile(int height);

	std::string getTitle();
	int getNumPages();
	int getNumChoices();

	void resetDraw();
	bool hasDrawLine();
	std::string getNextLine(int* row, bool* category);

	bool keyInput(TCOD_key_t key); /* returns true if selection should close and result is ready */
	Item* getItem();
	std::vector<Item*> getSelection();
	int getItemSymbol();
	std::vector<int> getSelectionSymbols();

};

#endif