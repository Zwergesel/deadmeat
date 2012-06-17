#ifndef _LEVEL_HPP
#define _LEVEL_HPP

#include <vector>
#include <sstream>
#include "tileset.hpp"
#include "utility.hpp"
#include "item.hpp"
#include "savegame.hpp"
#include "object.hpp"
#include "factory.hpp"

class Creature;
enum Tile;

enum RANDOM_LOCATION_FLAGS
{
  ANY_SPACE = 0,
  WALKABLE = 1,
  NO_CREATURE = 2,
  NO_ITEM = 4
};

struct TimelineAction
{
	Creature* actor;
	int time;
	TimelineAction(Creature* c, int t):actor(c),time(t) {};
};

bool operator<(TimelineAction a, TimelineAction b);

class Level
{
private:
	int width, height;
	Tile* map;
	bool* seen;
	std::vector<TimelineAction> creatures;
	std::vector<std::pair<Point, Item*> > items;
	// one object per position
	std::vector<std::pair<Point, Object> > objects;
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
	bool isSeen(Point pos);
	void setSeen(Point pos, bool isSeen);
	// returns creature at given position, NULL otherwise
	Creature* creatureAt(Point pos);
	std::vector<TimelineAction> getCreatures();
	void addCreature(Creature* c, int time);
	void removeCreature(Creature* c, bool del);
	std::vector<Creature*> getVisibleCreatures();
	std::vector<Item*> itemsAt(Point pos);
	std::vector<std::pair<Point, Item*> > getItems();
	void addObject(Object obj, Point pos);
	bool objectAt(Point p, Object& obj);
	void addItem(Item* i, Point pos);
	void removeItem(Item* i, bool del);

	Point chooseRandomPoint(std::vector<Point>& list, bool erase = false);
	std::vector<Point> getMatchingLocations(uint flags);
	Point getRandomLocation(uint flags);
	void populate(const RandomTable& spawns, int numCreatures);
	void placeItems(const RandomTable& items, int numItems);

	void buildTimeline(int time = -1);
	bool isPlayerTurn();
	void performCreatureTurn();

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

#endif