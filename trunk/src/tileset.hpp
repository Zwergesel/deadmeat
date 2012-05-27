#include <libtcod.hpp>

#ifndef _TILESET_HPP
#define _TILESET_HPP

enum Tile {
	TILE_CAVE_FLOOR,
	TILE_CAVE_WALL,
	TILE_STONE_FLOOR,
	TILE_MEADOW,
};

#define TILESET_LENGTH 4

struct TileInfo {
	bool passable;
	int symbol;
	TCODColor color, background;
	
	TileInfo():
		passable(false), symbol('?'), color(TCODColor::black), background(TCODColor::white){};
	TileInfo(bool p, int s, TCODColor c, TCODColor b):
		passable(p), symbol(s), color(c), background(b){};
};

class TileSet {
	
	public:
	TileInfo* info;
	TileSet();
	~TileSet();
	void setTile(Tile t, TileInfo inf);
	
};

#endif