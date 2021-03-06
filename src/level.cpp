#include <cassert>
#include <iostream>
#include "level.hpp"
#include "creature.hpp"
#include "player.hpp"
#include "world.hpp"
#include "savegame.hpp"
#include "items/weapon.hpp"
#include "tileset.hpp"
#include "items/corpse.hpp"

Level::Level()
{
	// for savegames only
	map = NULL;
	seen = NULL;
}

Level::Level(int w, int h):
	width(w), height(h)
{
	map = new Tile[w*h];
	seen = new bool[w*h];
	creatures.clear();
	items.clear();
	objects.clear();
	std::fill(map, map+w*h, TILE_CAVE_FLOOR);
	std::fill(seen, seen+w*h, false);
}

Level::~Level()
{
	if (map != NULL)
	{
		delete[] map;
		map = NULL;
	}
	if (seen != NULL)
	{
		delete[] seen;
		seen = NULL;
	}
	for (std::vector<TimelineAction>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if (it->actor != NULL)
		{
			delete it->actor;
			it->actor = NULL;
		}
	}
	creatures.clear();
	for (std::vector<std::pair<Point, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}
	items.clear();
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

bool Level::isSeen(Point pos)
{
	return seen[coord(pos)];
}

void Level::setSeen(Point pos, bool isSeen)
{
	seen[coord(pos)] = isSeen;
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

std::vector<Creature*> Level::getVisibleCreatures()
{
	std::vector<Creature*> visible;
	for (auto it = creatures.begin(); it != creatures.end(); it++)
	{
		Point pos = it->actor->getPos();
		if (world.fovMap->isInFov(pos.x, pos.y) && it->actor != world.player->getCreature())
		{
			visible.push_back(it->actor);
		}
	}
	return visible;
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

void Level::addObject(Object obj, Point p)
{
	objects.push_back(std::pair<Point, Object>(p, obj));
}

Object* Level::objectAt(Point p)
{
	for (auto it=objects.begin(); it<objects.end(); it++)
	{
		if (p == it->first)
		{
			return &it->second;
		}
	}
	return NULL;
}

Item* Level::addItem(Item* item, Point pos)
{
	// Try stacking the item
	for (auto it = items.begin(); it != items.end(); it++)
	{
		if (it->first == pos && item->canStackWith(it->second))
		{
			assert(it->second != item);
			it->second->changeAmount(item->getAmount());
			delete item;
			return it->second;
		}
	}
	// Stacking is not possible
	items.push_back(std::pair<Point, Item*>(pos, item));
	return item;
}

void Level::removeItem(Item* item, int num, bool del)
{
	for (auto it = items.begin(); it != items.end(); it++)
	{
		if (item == it->second)
		{
			assert(item->getAmount() >= num);
			if (item->getAmount() == num)
			{
				if (del) delete item;
				items.erase(it);
			}
			else if (!del)
			{
				item->changeAmount(-num);
				it->second = item->clone();
				item->setAmount(num);
			}
			else
			{
				item->changeAmount(-num);
			}
			break;
		}
	}
}

bool Level::isBlocking(Point pos)
{
	if (world.tileSet->isBlocking(map[coord(pos)])) return true;
	Object* obj = objectAt(pos);
	if (obj != NULL && obj->isBlocking()) return true;
	return false;
}

bool Level::isWalkable(Point pos)
{
	Object* obj = objectAt(pos);
	if (obj != NULL && obj->isBlocking()) return false;
	return world.tileSet->isWalkable(map[coord(pos)]);
}

bool Level::isTransparent(Point pos)
{
	Object* obj = objectAt(pos);
	if (obj != NULL && !obj->isTransparent()) return false;
	return world.tileSet->isTransparent(map[coord(pos)]);
}

std::vector<Point> Level::getMatchingLocations(uint flags, Point center, float radius)
{
	std::vector<Point> list;
	int lox(0), hix(width-1), loy(0), hiy(height-1);
	if (radius > 0)
	{
		lox = std::max(lox, static_cast<int>(center.x - radius));
		hix = std::min(hix, static_cast<int>(center.x + radius));
		loy = std::max(loy, static_cast<int>(center.y - radius));
		hiy = std::min(hiy, static_cast<int>(center.y + radius));
	}
	radius *= radius;
	for (int y=loy; y<=hiy; y++) for (int x=lox; x<=hix; x++)
		{
			if (radius > 0 && Point::sqlen(Point(x-center.x,y-center.y)) > radius) continue;
			if ((flags & WALKABLE) && !isWalkable(Point(x,y))) continue;
			else if ((flags & NO_CREATURE) && creatureAt(Point(x,y)) != NULL) continue;
			else if (flags & NO_ITEM)
			{
				bool hasItems = false;
				for (auto it = items.begin(); it != items.end(); it++)
				{
					if (it->first.x == x && it->first.y == y)
					{
						hasItems = true;
						break;
					}
				}
				if (hasItems) continue;
			}
			else if ((flags & NO_OBJECT) && objectAt(Point(x,y)) != NULL) continue;
			list.push_back(Point(x,y));
		}
	return list;
}

Point Level::chooseRandomPoint(std::vector<Point>& list, bool erase)
{
	assert(list.size() > 0);
	int index = rng->getInt(0,list.size()-1);
	if (erase)
	{
		std::swap(list[index], list.back());
		Point result = list.back();
		list.pop_back();
		return result;
	}
	return list[index];
}

Point Level::getRandomLocation(uint flags, Point center, float radius)
{
	auto list = getMatchingLocations(flags, center, radius);
	if (list.size() == 0) return center;
	return chooseRandomPoint(list);
}

void Level::populate(const RandomTable& spawns, int numCreatures)
{
	auto list = getMatchingLocations(WALKABLE | NO_CREATURE);
	while (numCreatures-->0)
	{
		Creature* c = factory.spawnCreature(spawns.getRandom());
		c->setPos(chooseRandomPoint(list,true));
		addCreature(c, world.time);
	}
}

void Level::placeItems(const RandomTable& items, int numItems)
{
	auto list = getMatchingLocations(WALKABLE);
	while (numItems-->0)
	{
		Item* i = factory.spawnItem(items.getRandom(), true);
		addItem(i, chooseRandomPoint(list));
	}
}

void Level::buildTimeline(int reset)
{
	if (reset >= 0)
	{
		for (auto it=creatures.begin(); it!=creatures.end(); it++) it->time = reset;
	}
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
		// corpse decay for current level and player inventory
		checkCorpses();
		world.player->checkCorpses();
		// player action; returns time the action took
		time = world.player->action();
		// TODO: this crashes when changing level!
		if (time > 0) creatures.back().actor->updateStatus(time);
	}
	else
	{
		// creature action; returns time the action took
		time = creatures.back().actor->action();
		creatures.back().actor->updateStatus(time);
		// creatures should never use zero time
		assert(time > 0);
	}
	// creatures might be empty now because the player left the level
	if (creatures.size() > 0)
	{
		// update heap
		creatures.back().time += time;
		push_heap(creatures.begin(), creatures.end());
	}
}

void Level::checkCorpses()
{
	for (int d=items.size()-1; d >= 0; d--)
	{
		if (items[d].second->getType() == ITEM_CORPSE)
		{
			Corpse* cr = static_cast<Corpse*>(items[d].second);
			if (cr->isDecaying()) removeItem(cr, cr->getAmount(), true);
		}
	}
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
	store ("map", map, width, height, seen);
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
	store("#objects", (int) objects.size());
	for (unsigned int d=0; d<objects.size(); d++)
	{
		store("_position", objects[d].first).ptr("_object", objects[d].second.save(sg));
	}
	sg << store;
	return id;
}

void Level::load(LoadBlock& load)
{
	int n;
	load ("width", width) ("height", height);
	map = new Tile[width*height];
	seen = new bool[width*height];
	load ("map", map, width, height, seen) ("#creatures", n);
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
	load ("#objects", n);
	while (n-->0)
	{
		Point pos;
		load("_position", pos);
		Object* obj = static_cast<Object*>(load.ptr("_object"));
		objects.push_back(std::make_pair(pos,*obj));
		delete obj;
	}
}