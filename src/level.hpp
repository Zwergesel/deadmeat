#ifndef _LEVEL_HPP
#define _LEVEL_HPP

#include <vector>
#include <sstream>
#include "tileset.hpp"
#include "utility.hpp"
#include "item.hpp"
#include "savegame.hpp"

class Creature;
enum Tile;

struct TimelineAction
{
	Creature* actor;
	int time;
	TimelineAction(Creature* c, int t):actor(c),time(t){};
};

bool operator<(TimelineAction a, TimelineAction b);

class Level
{
private:
	int width, height;
	Tile* map;
	std::vector<TimelineAction> creatures;
	std::vector<std::pair<Point, Item*> > items;
	inline int coord(Point pos);

	Level(); // for savegames only
	friend class Savegame;

public:
	Level(int width,int height);
	~Level();
	void setTile(Point pos, Tile t);
	Tile getTile(Point pos);
	int getWidth();
	int getHeight();
	// returns creature at given position, NULL otherwise
	Creature* creatureAt(Point pos);
	std::vector<TimelineAction> getCreatures();
	void addCreature(Creature* c, int time);
	void removeCreature(Creature* c, bool del);
	std::vector<Item*> itemsAt(Point pos);
	std::vector<std::pair<Point, Item*> > getItems();
	void addItem(Item* i, Point pos);
	void removeItem(Item* i, bool del);

	void buildTimeline();
	bool isPlayerTurn();
	void performCreatureTurn();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif