#include "level.hpp"
#include "utility.hpp"
#include "world.hpp"
#include <libtcod.hpp>
#include <algorithm>
#include <iostream>
#include <cassert>

Level::Level(int width, int height)
{
	this->width = width;
	this->height = height;
	this->map = new Tile[width*height];
	creatures.clear();
	timeline.clear();
	std::fill(map, map+width*height, TILE_CAVE_FLOOR);
}

Level::~Level()
{
	if (map != NULL)
	{
		delete[] map;
		map = NULL;
	}
	for (std::vector<Creature*>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		delete *it;
	}
}

inline int Level::coord(Point pos)
{
	return pos.y*width + pos.x;
}

void Level::setTile(Point pos, Tile t)
{
	if (map == NULL || pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) return;
	map[coord(pos)] = t;
}

Tile Level::getTile(Point pos)
{
	return map[coord(pos)];
}

int Level::getWidth()
{
	return width;
}

int Level::getHeight()
{
	return height;
}

Creature* Level::creatureAt(Point pos)
{
	for (std::vector<Creature*>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if ((*it)->getPos() == pos) return (*it);
	}
	return NULL;
}

std::vector<Creature*> Level::getCreatures()
{
	return creatures;
}

void Level::addCreature(Creature* c)
{
	creatures.push_back(c);
	// Put creature into timeline
	timeline.push_back(TimelineAction(0,c));	// TODO: should init with current time
	push_heap(timeline.begin(), timeline.end());
}

void Level::removeCreature(Creature* c)
{
	for (std::vector<Creature*>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if (*it == c) {
			delete *it;
			creatures.erase(it);
			break;
		}
	}
	
	// Remove creature from timeline and rebuild
	for (std::vector<TimelineAction>::iterator it=timeline.begin(); it<timeline.end(); it++)
	{
		if (it->actor == c) {
			timeline.erase(it);
			break;
		}
	}
	buildTimeline();
	
	delete c;
}

void Level::buildTimeline()
{
	make_heap(timeline.begin(), timeline.end());
}

bool Level::isCreatureTurn(int playerTime)
{
	return (!timeline.empty() && timeline.front().time < playerTime);
}

void Level::performCreatureTurn()
{
	pop_heap(timeline.begin(), timeline.end());
	int time = timeline.back().actor->action(this, world.player);
	// action(...) returns the time the action took; update heap
	assert(time > 0);
	timeline.back().time += time;
	push_heap(timeline.begin(), timeline.end());
}

bool operator<(TimelineAction a, TimelineAction b)
{
	// Max heap, but we want minimum time
	return a.time > b.time;
}