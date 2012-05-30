#ifndef _LEVEL_HPP
#define _LEVEL_HPP

#include <vector>
#include "tileset.hpp"
#include "utility.hpp"

class Creature;

struct TimelineAction
{
	int time;
	Creature* actor;
	TimelineAction(int t, Creature* c):time(t),actor(c) {};
};

bool operator<(TimelineAction a, TimelineAction b);

class Level
{
private:
	int width, height;
	Tile* map;
	std::vector<Creature*> creatures;
	std::vector<TimelineAction> timeline;

	inline int coord(Point pos);

public:
	Level(int width,int height);
	~Level();
	void setTile(Point pos, Tile t);
	Tile getTile(Point pos);
	int getWidth();
	int getHeight();
	// returns creature at given position, NULL otherwise
	Creature* creatureAt(Point pos);
	std::vector<Creature*> getCreatures();
	void addCreature(Creature* c);
	void removeCreature(Creature* c);

	void buildTimeline();
	bool isPlayerTurn();
	void performCreatureTurn();
};

#endif