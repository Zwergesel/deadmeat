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

void Level::display(int offsetX, int offsetY)
{
	int startX = (offsetX < 0) ? -offsetX : 0;
	int startY = (offsetY < 0) ? -offsetY : 0;
	int rangeX = std::min(width - offsetX, TCODConsole::root->getWidth());
	int rangeY = std::min(height - offsetY, TCODConsole::root->getHeight() - 1);
	for (int y=startY; y<rangeY; y++)
	{
		for (int x=startX; x<rangeX; x++)
		{
			// TODO: put globalTileSet into a World class and only keep World global
			TileInfo inf = globalTileSet.info[map[coord(x+offsetX,y+offsetY)]];
			TCODConsole::root->putCharEx(x, y, inf.symbol, inf.color, inf.background);
		}
	}
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
