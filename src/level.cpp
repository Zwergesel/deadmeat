#include "level.hpp"
#include <libtcod.hpp>
#include <algorithm>
#include <iostream>

Level::Level(int width, int height) 
{
	this->width = width;
	this->height = height;
	this->map = new Tile[width*height];
	std::fill(map, map+width*height, Tile('.', true));
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
		  //std::cerr << coord(x+offsetX,y+offsetY) << std::endl;
      TCODConsole::root->setChar(x, y, map[coord(x+offsetX,y+offsetY)].symbol);
	  }
	}
}

void Level::setTile(int x, int y, Tile t)
{
  if(map == NULL || x < 0 || y < 0 || x >= width || y >= height) return;
  map[coord(x, y)] = t;
}

Tile Level::getTile(int x, int y)
{
  return map[coord(x, y)];
}

void Level::setSize(int width, int height)
{
  this->width = width;
  this->height = height;
}

int Level::getWidth()
{
  return width;
}

int Level::getHeight()
{
  return height;
}
