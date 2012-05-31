#ifndef _WORLD_HPP
#define _WORLD_HPP

#include <deque>
#include <vector>
#include "utility.hpp"
#include "item.hpp"
#include "itemselection.hpp"

class Player;
class Creature;
class Level;
class TileSet;

struct Viewport
{
	int x,y,width,height;
	Viewport():x(0),y(0),width(1),height(1) {};
	Viewport(int x, int y, int w, int h):x(x),y(y),width(w),height(h) {};
};

class World
{
private:
	std::deque<std::string> messageQueue;

public:
	TileSet* tileSet;
	Player* player;
	Level** levels;
	int currentLevel;
	Point levelOffset;
	Viewport viewLevel, viewMsg;
	bool requestQuit;
	int substateCounter;

	World();
	~World();

	void addMessage(std::string m, bool forceBreak=false);
	int getNumMessages();
	void popMessage();
	void drawMessage();

	void toggleFullscreen();

	void drawLevel(Level* l, Point offset, Viewport view);
	void drawItem(Item* i, Point offset, Viewport view);
	void drawCreature(Creature* c, Point offset, Viewport view);
	void drawWorld();
	void drawItemList(int page, std::string title, std::vector<Item*> items);
	void drawItemList(int page, std::string title, std::vector<std::pair<int, Item*> > items);
	void drawItemList(int page, std::string title, std::vector<std::pair<int, Item*> > items, std::vector<ITEM_TYPE> filter);
	void drawItemSelection(ItemSelection& sel);
	void drawInventory(int page);
};

// Global world variable
extern World world;

#endif