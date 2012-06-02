#ifndef _WORLD_HPP
#define _WORLD_HPP

#include <deque>
#include <vector>
#include <sstream>
#include "utility.hpp"
#include "item.hpp"
#include "itemselection.hpp"

class Player;
class Creature;
class Level;
class TileSet;
class Savegame;
class LoadBlock;

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
	Viewport viewLevel, viewMsg, viewItemList;
	bool requestQuit;
	ItemSelection itemSelection;

	World();
	~World();

	void addMessage(std::string m, bool forceBreak=false);
	int getNumMessages();
	void popMessage();
	void drawMessage();
	unsigned char drawBlockingWindow(std::string title, std::string text, std::vector<unsigned char> acceptedKeys);

	void toggleFullscreen();

	void drawLevel(Level* l, Point offset, Viewport view);
	void drawItem(Item* i, Point pos, Point offset, Viewport view);
	void drawCreature(Creature* c, Point offset, Viewport view);
	void drawWorld();
	void drawInfo();
	void drawItemSelection(ItemSelection& sel);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

// Global world variable
extern World world;

#endif