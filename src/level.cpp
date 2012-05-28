#include "level.hpp"
#include <libtcod.hpp>
#include <algorithm>
#include <iostream>

Level::Level(int width, int height)
{
	this->width = width;
	this->height = height;
	this->map = new Tile[width*height];
	creatures.clear();
	std::fill(map, map+width*height, TILE_CAVE_FLOOR);
}

Level::~Level()
{
	if (map != NULL)
	{
		delete[] map;
		map = NULL;
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
	for (std::vector<Creature>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		if ((*it).getPos() == pos) return &(*it);
	}
	return NULL;
}

std::vector<Creature>* Level::getCreatures()
{
	return &creatures;
}

void Level::addCreature(Creature c)
{
	creatures.push_back(c);
}
