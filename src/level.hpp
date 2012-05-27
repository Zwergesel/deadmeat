#include "tileset.hpp"

#ifndef _LEVEL_HPP
#define _LEVEL_HPP

class Level
{
private:
	int width, height;
	Tile* map;

	inline int coord(int x, int y);

public:
	Level(int width,int height);
	~Level();
	void setTile(int x, int y, Tile t);
	Tile getTile(int x, int y);
	int getWidth();
	int getHeight();
};

#endif