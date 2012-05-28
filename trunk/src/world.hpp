#ifndef _WORLD_HPP
#define _WORLD_HPP

#include <string>
#include <libtcod.hpp>
#include <deque>
#include "tileset.hpp"
#include "level.hpp"
#include "creature.hpp"

struct Point
{
	int x, y;
	Point():x(0),y(0){};
	Point(int x, int y):x(x),y(y){};
};

struct Viewport
{
	int x,y,width,height;
	Viewport():x(0),y(0),width(0),height(0){};
	Viewport(int x, int y, int w, int h):x(x),y(y),width(w),height(h){};
};

class World
{
	public:
	TileSet* tileSet;
	Player* player;
	Level** levels;
	int currentLevel;
	Point levelOffset;

	World();
	~World();
	void drawLevel(Level* l, Point offset, Viewport view);
	void drawCreature(Creature* c, Point offset, Viewport view);
	int drawMessages(std::deque<std::string>* m, Viewport view);
	void drawPlayer(Player* p, Point offset, Viewport view);
	
	void debugDrawWorld(Goblin* g, FailWhale* w);
};

// Global world variable
extern World world;

#endif