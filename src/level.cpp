#include <cassert>
#include "level.hpp"
#include "creature.hpp"
#include "player.hpp"
#include "world.hpp"

Level::Level(int width, int height)
{
	this->width = width;
	this->height = height;
	this->map = new Tile[width*height];
	creatures.clear();
	timeline.clear();
	items.clear();
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
	for (std::vector<Item*>::iterator it=items.begin(); it<items.end(); it++)
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
	// Set level
	c->setLevel(this);
}

void Level::removeCreature(Creature* c, bool del)
{
	for (std::vector<Creature*>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if (*it == c)
		{
			creatures.erase(it);
			break;
		}
	}

	// Remove creature from timeline and rebuild
	for (std::vector<TimelineAction>::iterator it=timeline.begin(); it<timeline.end(); it++)
	{
		if (it->actor == c)
		{
			timeline.erase(it);
			break;
		}
	}
	buildTimeline();

	if (del && c != NULL)
	{
		delete c;
		c = NULL;
	}
}

std::vector<Item*> Level::itemsAt(Point pos)
{
	std::vector<Item*> ret;
	for (std::vector<Item*>::iterator it=items.begin(); it<items.end(); it++)
	{
		if ((*it)->getPos() == pos) ret.push_back(*it);
	}
	return ret;
}

std::vector<Item*> Level::getItems()
{
	return items;
}

void Level::addItem(Item* i)
{
	items.push_back(i);
}

void Level::removeItem(Item* i, bool del)
{
	for (std::vector<Item*>::iterator it=items.begin(); it<items.end(); it++)
	{
		if (*it == i)
		{
			items.erase(it);
			break;
		}
	}
  if (del && i != NULL) 
  {
    delete i;
    i = NULL;
  }
}

void Level::buildTimeline()
{
	make_heap(timeline.begin(), timeline.end());
}

bool Level::isPlayerTurn()
{
	return (!timeline.empty() && timeline.front().actor->isControlled());
}

void Level::performCreatureTurn()
{
	// pop_heap puts currently active creature to the back of the vector
	pop_heap(timeline.begin(), timeline.end());
	int time;
	if (timeline.back().actor->isControlled())
	{
		// player action; returns time the action took
		time = world.player->action(this);
	}
	else
	{
		// creature action; returns time the action took
		time = timeline.back().actor->action();
		// creatures should never use zero time
		assert(time > 0);
	}
	// update heap
	timeline.back().time += time;
	push_heap(timeline.begin(), timeline.end());
}

bool operator<(TimelineAction a, TimelineAction b)
{
	// Max heap, but we want minimum time
	return a.time > b.time;
}