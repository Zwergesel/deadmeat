#include <cassert>
#include "level.hpp"
#include "creature.hpp"
#include "player.hpp"
#include "world.hpp"
#include "savegame.hpp"
#include "items/weapon.hpp"
#include "tileset.hpp"

Level::Level()
{
	// for savegames only
}

Level::Level(int width, int height)
{
	this->width = width;
	this->height = height;
	this->map = new Tile[width*height];
	creatures.clear();
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
	for (std::vector<TimelineAction>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if (it->actor != NULL)
		{
			delete it->actor;
			it->actor = NULL;
		}
	}
	for (std::vector<std::pair<Point, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
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
	for (std::vector<TimelineAction>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if (it->actor->getPos() == pos) return it->actor;
	}
	return NULL;
}

std::vector<TimelineAction> Level::getCreatures()
{
	return creatures;
}

void Level::addCreature(Creature* c, int time)
{
	creatures.push_back(TimelineAction(c, time));
	push_heap(creatures.begin(), creatures.end());
	// Set level
	c->setLevel(this);
}

void Level::removeCreature(Creature* c, bool del)
{
	for (std::vector<TimelineAction>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if (it->actor == c)
		{
			creatures.erase(it);
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
	for (std::vector<std::pair<Point, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		if ((*it).first == pos) ret.push_back((*it).second);
	}
	return ret;
}

std::vector<std::pair<Point, Item*> > Level::getItems()
{
	return items;
}

void Level::addItem(Item* i, Point pos)
{
	items.push_back(std::pair<Point, Item*>(pos, i));
}

void Level::removeItem(Item* i, bool del)
{
	for (std::vector<std::pair<Point, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		if ((*it).second == i)
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
	std::make_heap(creatures.begin(), creatures.end());
}

bool Level::isPlayerTurn()
{
	return (!creatures.empty() && creatures.front().actor->isControlled());
}

void Level::performCreatureTurn()
{
	// pop_heap puts currently active creature to the back of the vector
	pop_heap(creatures.begin(), creatures.end());
	world.time = creatures.back().time;
	int time;
	if (creatures.back().actor->isControlled())
	{
		// player action; returns time the action took
		time = world.player->action();
	}
	else
	{
		// creature action; returns time the action took
		time = creatures.back().actor->action();
		// creatures should never use zero time
		assert(time > 0);
	}
	// update heap
	creatures.back().time += time;
	push_heap(creatures.begin(), creatures.end());
}

bool operator<(TimelineAction a, TimelineAction b)
{
	// Max heap, but we want minimum time
	if (a.time != b.time) return a.time > b.time;
	// In case of a tie player creatures always go first
	if (a.actor->isControlled() != b.actor->isControlled()) return b.actor->isControlled();
	return false;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Level::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Level", id);
	store ("width", width) ("height", height);
	store ("map", map, width, height);
	store ("#creatures", (int) creatures.size());
	for (unsigned int d=0; d<creatures.size(); d++)
	{
		store.ptr("_creature", creatures[d].actor->save(sg));
		store("_time", creatures[d].time);
	}
	store ("#items", (int) items.size());
	for (unsigned int d=0; d<items.size(); d++)
	{
		store ("_position", items[d].first).ptr("_item", items[d].second->save(sg));
	}
	sg << store;
	return id;
}

void Level::load(LoadBlock& load)
{
	int n;
	load ("width", width) ("height", height);
	map = new Tile[width*height];
	load ("map", map, width, height) ("#creatures", n);
	while (n-->0)
	{
		int time;
		Creature* c = static_cast<Creature*>(load.ptr("_creature"));
		load ("_time", time);
		addCreature(c, time);
	}
	load ("#items", n);
	while (n-->0)
	{
		Point pos;
		load("_position", pos);
		Item* item = static_cast<Item*>(load.ptr("_item"));
		items.push_back(std::make_pair(pos,item));
	}
}