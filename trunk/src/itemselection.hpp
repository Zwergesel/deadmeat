#ifndef _ITEMSELECTION_HPP
#define _ITEMSELECTION_HPP

#include <vector>
#include <set>
#include <string>
#include "item.hpp"

struct ItemListInfo
{
	int row;
	std::string text;
	bool centered;
	ItemListInfo():row(0),text(""),centered(false){};
	ItemListInfo(int r, std::string t, bool c):row(r),text(t),centered(c){};
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
	
	int compiledFor;
	std::vector<ItemListInfo> compiledStrings;
	std::vector<int> pageStart;
	
	bool removeAnonItem(Item* item);
	bool removeNamedItem(std::pair<int,Item*> item);
	
public:
	ItemSelection(const std::vector<Item*>& choices, std::string title, bool multiple = false, bool sort = true);
	ItemSelection(const std::vector<std::pair<int,Item*> >& choices, std::string title, bool multiple = false, bool sort = true);
	
	ItemSelection* filterType(ITEM_TYPE type);
	int runFilter();
	std::string getTitle();
	void setPage(int page);
	int getPage();
	int getNumPages();
	void compile(int height);
	std::vector<ItemListInfo> getPageList();
	
};

#endif