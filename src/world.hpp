#ifndef _WORLD_HPP
#define _WORLD_HPP

#include <string>
#include <libtcod.hpp>
#include <deque>
#include "tileset.hpp"
#include "level.hpp"
#include "utility.hpp"
#include "creature.hpp"

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

	World();
	~World();

	void addMessage(std::string m);
	int getNumMessages();
	void popMessage();
	void drawMessage();

	void drawLevel(Level* l, Point offset, Viewport view);
	void drawCreature(Creature c, Point offset, Viewport view);
	void drawPlayer(Player* p, Point offset, Viewport view);

	void debugDrawWorld(Goblin* g, FailWhale* w);
};

// Global world variable
extern World world;

#endif