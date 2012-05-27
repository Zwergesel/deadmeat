#include "level.hpp"
#include <libtcod.hpp>
#include <algorithm>
#include <iostream>

Level::Level(int width, int height)
{
	this->width = width;
	this->height = height;
	this->map = new Tile[width*height];
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

inline int Level::coord(int x, int y)
{
	return y*width + x;
}

void Level::setTile(int x, int y, Tile t)
{
	if (map == NULL || x < 0 || y < 0 || x >= width || y >= height) return;
	map[coord(x, y)] = t;
}

Tile Level::getTile(int x, int y)
{
	return map[coord(x, y)];
}

int Level::getWidth()
{
	return width;
}

int Level::getHeight()
{
	return height;
}
